#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <memory.h>

__attribute__((always_inline))
paddr_t mmu_translate(vaddr_t vaddr, int type) {
#define CHECK_PTE(PTE, PermR, PermW, PermX, TYPE) \
    do { \
        if ((PTE).R < (PermR) || (PTE).W < (PermW) || (PTE).X < (PermX) || !((PTE).V) ) { \
          Error("Permission Denied for type %d", MEM_TYPE_ ## TYPE); \
          assert(0); \
        } \
    } while(0)

#define UPDATE_PTE(PTE, BitA, BitD) \
    do { \
        (PTE).A = (BitA); \
        (PTE).D = (BitD); \
        paddr_write((PTE ## _addr), sizeof(PTE_Decode), (PTE).val); \
    } while (0)

    VA_Decode VA = (VA_Decode)vaddr;
    CSR_Decode SATP = (CSR_Decode)csr(satp);
    paddr_t lv1_PTE_addr = (SATP.satp.PPN << 12) + (VA.VPN_1 << 2);
    PTE_Decode lv1_PTE = (PTE_Decode)paddr_read(lv1_PTE_addr, sizeof(PTE_Decode));
    paddr_t lv2_PTE_addr = (lv1_PTE.PPN << 12) + (VA.VPN_0 << 2);
    PTE_Decode lv2_PTE = (PTE_Decode)paddr_read(lv2_PTE_addr, sizeof(PTE_Decode));
    switch (type) {
        case MEM_TYPE_READ:   CHECK_PTE(lv2_PTE, 1, 0, 0, READ);   UPDATE_PTE(lv2_PTE, 1, 0); break;
        case MEM_TYPE_WRITE:  CHECK_PTE(lv2_PTE, 1, 1, 0, WRITE);  UPDATE_PTE(lv2_PTE, 1, 1); break;
        case MEM_TYPE_IFETCH: CHECK_PTE(lv2_PTE, 1, 0, 1, IFETCH); UPDATE_PTE(lv2_PTE, 1, 0); break;
    }
    return (lv2_PTE_addr << 12) + VA.offset;
}
