#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <cpu/reg.h>
#include <utils/state.h>

#define IN_PMEM(ADDR) \
    ((ADDR) >= CONFIG_MBASE && (ADDR) < CONFIG_MBASE + CONFIG_MSIZE)

typedef void * haddr_t;
typedef uint32_t PTE;

typedef union __attribute__((packed)) {
    struct { word_t V : 1, R : 1, W : 1, X : 1, U : 1, G : 1, A : 1, D : 1, RSW : 2, PPN : 22; };
    word_t RAW;
} PTE_Decode;

typedef union __attribute__((packed)) {
    struct { vaddr_t offset : 12, VPN_0 : 10, VPN_1 : 10; };
    vaddr_t addr;
} VA_Decode;

typedef union __attribute__((packed)) {
    struct { paddr_t offset : 12, PPN : 20; }; // Not 22 for PPN for now...
    paddr_t addr;
} PA_Decode;

enum { MEM_TYPE_READ, MEM_TYPE_WRITE, MEM_TYPE_EXEC };

// Should not use else where...
#define GET_PTE(PTE_addr) \
    ((PTE_Decode)paddr_read((PTE_addr), sizeof(PTE)))

#define VERIFY_PTE(PTE, PermR, PermW, PermX) \
    do { \
        if (!(PTE).V || (PTE).R < (PermR) || (PTE).W < (PermW) || (PTE).X < (PermX)) { \
            Error("Permission Denied!"); \
            SET_STATE(ABORT); \
        } \
    } while (0)

#define UPDATE_PTE(PTE, BitA, BitD) \
    do { \
        (PTE).A = BitA; \
        (PTE).D = BitD; \
        paddr_write((PTE ## _addr), sizeof(PTE), (PTE).RAW); \
    } while (0)

// Publie MMU_TRANSLATE Macro
#define MMU_TRANSLATE(VADDR, TYPE) \
    ({ \
        paddr_t lv1_PTE_addr = (((CSR_Decode)csr(satp)).satp.PPN << 12) + (((VA_Decode)(VADDR)).VPN_1 << 2); \
        PTE_Decode lv1_PTE = GET_PTE(lv1_PTE_addr); \
        paddr_t lv2_PTE_addr = (lv1_PTE.PPN << 12) + (((VA_Decode)(VADDR)).VPN_0 << 2); \
        PTE_Decode lv2_PTE = GET_PTE(lv2_PTE_addr); \
        switch (TYPE) { \
            case MEM_TYPE_READ:  VERIFY_PTE(lv2_PTE, 1, 0, 0); UPDATE_PTE(lv2_PTE, 1, 0); break; \
            case MEM_TYPE_WRITE: VERIFY_PTE(lv2_PTE, 1, 1, 0); UPDATE_PTE(lv2_PTE, 1, 1); break; \
            case MEM_TYPE_EXEC:  VERIFY_PTE(lv2_PTE, 1, 0, 1); UPDATE_PTE(lv2_PTE, 1, 0); break; \
            default: assert(0); \
        } \
        ((lv2_PTE.PPN << 12) + ((VA_Decode)(VADDR)).offset); \
    })

enum { MMU_ON, MMU_OFF };

#define MMU_STATUS() (((CSR_Decode)csr(satp)).satp.MODE ? MMU_ON : MMU_OFF)

void *guest_to_host(paddr_t addr);
paddr_t host_to_guest(const void *addr);

word_t host_read(const haddr_t haddr, size_t len);
void host_write(haddr_t haddr, size_t len, word_t data);

word_t paddr_read(paddr_t addr, size_t len);
void paddr_write(paddr_t addr, size_t len, word_t data);

word_t vaddr_read(vaddr_t addr, size_t len);
void vaddr_write(vaddr_t write, size_t len, word_t data);
word_t vaddr_fetchinst(vaddr_t addr, size_t len);

#endif