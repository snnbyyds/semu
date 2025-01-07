#include <cpu/reg.h>
#include <device/mmio.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

static uint8_t memory[CONFIG_MSIZE] PG_ALIGN;
void *pmem = memory;

/* Translate vaddr to paddr (RV32 Sv32)
 * @vaddr: virtual address
 * @type: access type
 * @return: physical address
 */
__attribute__((always_inline))
static inline paddr_t isa_mmu_translate(vaddr_t vaddr, mem_access_t type) {
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

    return ((*lv2_PTE & PTE_PPN) << 2) + (vaddr & VA_OFFSET);
}

/* Read from physical address */
#define PADDR_READ_IMPL(SIZE, TYPE, LEN) \
    __attribute__((always_inline)) \
    static inline TYPE paddr_read_ ## SIZE(paddr_t addr) { \
        return likely(IN_PMEM(addr)) ? \
            *(TYPE *)GUEST_TO_HOST(addr) : \
            mmio_read(addr, LEN); \
    }

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

PADDR_WRITE_IMPL(w, uint32_t, 4)
PADDR_WRITE_IMPL(s, uint16_t, 2)
PADDR_WRITE_IMPL(b, uint8_t, 1)

/* Read from virtual address */
#define VADDR_READ_IMPL(SIZE, TYPE) \
    TYPE vaddr_read_ ## SIZE(vaddr_t addr) { \
        if (cpu.csr_satp) addr = isa_mmu_translate(addr, MEM_TYPE_READ); \
        return paddr_read_ ## SIZE(addr); \
    }

VADDR_READ_IMPL(w, uint32_t)
VADDR_READ_IMPL(s, uint16_t)
VADDR_READ_IMPL(b, uint8_t)

/* Write to virtual address */
#define VADDR_WRITE_IMPL(SIZE, TYPE) \
    void vaddr_write_ ## SIZE(vaddr_t addr, TYPE data) { \
        if (cpu.csr_satp) addr = isa_mmu_translate(addr, MEM_TYPE_WRITE); \
        paddr_write_ ## SIZE(addr, data); \
    }

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

void init_memory() {
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    assert(urandom_fd != -1);
    ssize_t nread = read(urandom_fd, memory, CONFIG_MSIZE);
    assert(nread == CONFIG_MSIZE);
    close(urandom_fd);
    Log("Memory initialized at 0x%" PRIaddr "", (paddr_t)CONFIG_MBASE);
}
