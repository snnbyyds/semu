#include <memory.h>
#include <cpu/reg.h>
#include <utils/bit.h>

#define DECODE(N, FIELD) (get_field(N, FIELD, FIELD ## _SHIFT))
#define SET(N, FIELD, VAL) (set_field(N, FIELD, FIELD ## _SHIFT, VAL))

#define UPDATE_PTE(_PTE, BitA, BitD) ({ \
    PTE t = SET(_PTE, PTE_A, BitA); \
    t = SET(t, PTE_D, BitD); \
    paddr_write(_PTE ## _addr, sizeof(PTE), t); \
})

#define VERIFY_PTE(PTE, PermR, PermW, PermX) ({ \
    assert((PTE & PTE_V) && DECODE(PTE, PTE_R) >= PermR && DECODE(PTE, PTE_W) >= PermW && DECODE(PTE, PTE_X) >= PermX); \
})

/* Check current MMU state
 * @return: MMU state
 */
mmu_state_t isa_mmu_check() {
    return csr(SATP) ? MMU_TRANSLATE : MMU_DIRECT;
}

/* Walk through page tables to translate vaddr to paddr
 * @vaddr: virtual address
 * @type: access type
 * @return: physical address
 */
paddr_t isa_mmu_translate(vaddr_t vaddr, mem_access_t type) {
    paddr_t lv1_PTE_addr = (get_csr_field(SATP, SATP_PPN, SATP_PPN_SHIFT) << 12) + (DECODE(vaddr, VA_VPN_1) << 2);
    PTE lv1_PTE = paddr_read(lv1_PTE_addr, sizeof(PTE));
    paddr_t lv2_PTE_addr = (DECODE(lv1_PTE, PTE_PPN) << 12) + (DECODE(vaddr, VA_VPN_0) << 2);
    PTE lv2_PTE = paddr_read(lv2_PTE_addr, sizeof(PTE));
    switch (type) {
        case MEM_TYPE_READ:   VERIFY_PTE(lv2_PTE, 1, 0, 0); UPDATE_PTE(lv2_PTE, 1, 0); break;
        case MEM_TYPE_WRITE:  VERIFY_PTE(lv2_PTE, 1, 1, 0); UPDATE_PTE(lv2_PTE, 1, 1); break;
        case MEM_TYPE_IFETCH: VERIFY_PTE(lv2_PTE, 1, 0, 1); UPDATE_PTE(lv2_PTE, 1, 0); break;
        default: assert(0);
    }
    return (DECODE(lv2_PTE, PTE_PPN) << 12) + DECODE(vaddr, VA_OFFSET);
}
