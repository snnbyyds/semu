/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cpu/reg.h>
#include <device/mmio.h>
#include <memory.h>
#include <unistd.h>
#include <utils/uthash.h>
#include <fcntl.h>

static uint8_t memory[CONFIG_MSIZE] PG_ALIGN;
void *pmem = memory;

/* MMU and TLB */

typedef struct {
    PTE pte; // serves as value
    uint32_t vpn; // serves as key
    UT_hash_handle hh;
} tlb_entry_t;

static tlb_entry_t *tlb = NULL;

#define NR_TLB_ENTRY 512

void isa_tlb_flush() {
    tlb_entry_t *cur = NULL;
    tlb_entry_t *nxt = NULL;
    HASH_ITER(hh, tlb, cur, nxt) {
        HASH_DEL(tlb, cur);
        free(cur);
    }
}

__attribute__((always_inline))
static inline void isa_tlb_add_mapping(uint32_t vpn, PTE pte) {
    tlb_entry_t *cur = NULL;
    tlb_entry_t *nxt = NULL;
    HASH_FIND_INT(tlb, &vpn, cur);
    if (cur) {
        HASH_DEL(tlb, cur);
        cur->vpn = vpn;
        cur->pte = pte;
        HASH_ADD_INT(tlb, vpn, cur);
    } else {
        if (HASH_COUNT(tlb) == NR_TLB_ENTRY) {
            HASH_ITER(hh, tlb, cur, nxt) {
                HASH_DEL(tlb, cur);
                free(cur);
                break;
            }
        }
        tlb_entry_t *new_entry = (tlb_entry_t *)malloc(sizeof(tlb_entry_t));
        new_entry->pte = pte;
        new_entry->vpn = vpn;
        HASH_ADD_INT(tlb, vpn, new_entry);
    }
}

__attribute__((always_inline))
static inline PTE isa_get_cached_pte(uint32_t vpn) {
    tlb_entry_t *cur = NULL;
    HASH_FIND_INT(tlb, &vpn, cur);
    if (cur) {
        HASH_DEL(tlb, cur);
        HASH_ADD_INT(tlb, vpn, cur);
        return cur->pte;
    }
    return -1;
}

__attribute__((always_inline))
static inline paddr_t isa_pgtable_walk(vaddr_t vaddr, mem_access_t type) {
    paddr_t lv1_PTE_addr =
        ((cpu.csr_satp & SATP_PPN) << 12)
        + ((vaddr & VA_VPN_1) >> 20);
    PTE *lv1_PTE = GUEST_TO_HOST(lv1_PTE_addr);

    paddr_t lv2_PTE_addr =
        ((*lv1_PTE & PTE_PPN) << 2)
        + ((vaddr & VA_VPN_0) >> 10);
    PTE *lv2_PTE = GUEST_TO_HOST(lv2_PTE_addr);

    switch (type) {
        case MEM_TYPE_READ:
            assert(*lv2_PTE & (PTE_V | PTE_R));
            *lv2_PTE |= PTE_A;
            break;
        case MEM_TYPE_WRITE:
            assert(*lv2_PTE & (PTE_V | PTE_R | PTE_W));
            *lv2_PTE |= (PTE_A | PTE_D);
            break;
        case MEM_TYPE_IFETCH:
            assert(*lv2_PTE & (PTE_V | PTE_R | PTE_X));
            *lv2_PTE |= PTE_A;
            break;
        default:
            assert(0);
    }

    isa_tlb_add_mapping((vaddr & VA_VPN) >> VA_VPN_SHIFT, *lv2_PTE);

    return ((*lv2_PTE & PTE_PPN) << 2) + (vaddr & VA_OFFSET);
}

/* Translate vaddr to paddr (RV32 Sv32)
 * @vaddr: virtual address
 * @type: access type
 * @return: physical address
 */
__attribute__((always_inline))
static inline paddr_t isa_mmu_translate(vaddr_t vaddr, mem_access_t type) {
    PTE pte = -1;
    uint32_t vpn = (vaddr & VA_VPN) >> VA_VPN_SHIFT;
    if ((pte = isa_get_cached_pte(vpn)) != -1) {
        return ((pte & PTE_PPN) << 2) + (vaddr & VA_OFFSET);
    }
    return isa_pgtable_walk(vaddr, type);
}

/* Common physical address read and write */

/* Read from physical address */
#define PADDR_READ_IMPL(SIZE, TYPE, LEN) \
    __attribute__((always_inline)) \
    static inline TYPE paddr_read_ ## SIZE(paddr_t addr) { \
        return likely(IN_PMEM(addr)) ? \
            *(TYPE *)GUEST_TO_HOST(addr) : \
            mmio_read(addr, LEN); \
    }

PADDR_READ_IMPL(d, uint64_t, 8)
PADDR_READ_IMPL(w, uint32_t, 4)
PADDR_READ_IMPL(s, uint16_t, 2)
PADDR_READ_IMPL(b, uint8_t, 1)

/* Write to physical address */
#define PADDR_WRITE_IMPL(SIZE, TYPE, LEN) \
    __attribute__((always_inline)) \
    static inline void paddr_write_ ## SIZE(paddr_t addr, TYPE data) { \
        if (likely(IN_PMEM(addr))) { \
            *(TYPE *)GUEST_TO_HOST(addr) = data; \
        } else { \
            mmio_write(addr, LEN, data); \
        } \
    }

PADDR_WRITE_IMPL(d, uint64_t, 8)
PADDR_WRITE_IMPL(w, uint32_t, 4)
PADDR_WRITE_IMPL(s, uint16_t, 2)
PADDR_WRITE_IMPL(b, uint8_t, 1)

/* Common virtual address read and write */

/* Read from virtual address */
#define VADDR_READ_IMPL(SIZE, TYPE) \
    TYPE vaddr_read_ ## SIZE(vaddr_t addr) { \
        if (cpu.csr_satp) addr = isa_mmu_translate(addr, MEM_TYPE_READ); \
        return paddr_read_ ## SIZE(addr); \
    }

VADDR_READ_IMPL(d, uint64_t)
VADDR_READ_IMPL(w, uint32_t)
VADDR_READ_IMPL(s, uint16_t)
VADDR_READ_IMPL(b, uint8_t)

/* Write to virtual address */
#define VADDR_WRITE_IMPL(SIZE, TYPE) \
    void vaddr_write_ ## SIZE(vaddr_t addr, TYPE data) { \
        if (cpu.csr_satp) addr = isa_mmu_translate(addr, MEM_TYPE_WRITE); \
        paddr_write_ ## SIZE(addr, data); \
    }

VADDR_WRITE_IMPL(d, uint64_t)
VADDR_WRITE_IMPL(w, uint32_t)
VADDR_WRITE_IMPL(s, uint16_t)
VADDR_WRITE_IMPL(b, uint8_t)

/* Read from virtual address for an instruction */
word_t vaddr_ifetch(vaddr_t addr) {
    if (cpu.csr_satp) {
        addr = isa_mmu_translate(addr, MEM_TYPE_IFETCH);
    }
    return paddr_read_w(addr);
}

/* Memory Initialization */

void init_memory() {
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    assert(urandom_fd != -1);
    ssize_t nread = read(urandom_fd, memory, CONFIG_MSIZE);
    assert(nread == CONFIG_MSIZE);
    close(urandom_fd);
    Log("Memory initialized at 0x%" PRIaddr "", (paddr_t)CONFIG_MBASE);
}
