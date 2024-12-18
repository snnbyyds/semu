#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

#define IN_PMEM(ADDR) \
    ((ADDR) >= CONFIG_MBASE && (ADDR) < CONFIG_MBASE + CONFIG_MSIZE)

typedef void * haddr_t;

typedef union __attribute__((packed)) {
    struct { word_t V : 1, R : 1, W : 1, X : 1, U : 1, G : 1, A : 1, D : 1, RSW : 2, PPN : 22; };
    word_t val;
} PTE_Decode;

typedef union __attribute__((packed)) {
    struct { vaddr_t offset : 12, VPN_0 : 10, VPN_1 : 10; };
    vaddr_t addr;
} VA_Decode;

typedef union __attribute__((packed)) {
    struct { paddr_t offset : 12, PPN : 20; }; // Not 22 for PPN for now...
    paddr_t addr;
} PA_Decode;

enum { MMU_ON, MMU_OFF };
enum { MEM_TYPE_READ, MEM_TYPE_WRITE, MEM_TYPE_IFETCH };

void *guest_to_host(paddr_t addr);
paddr_t host_to_guest(const void *addr);

word_t host_read(const haddr_t haddr, size_t len);
void host_write(haddr_t haddr, size_t len, word_t data);

word_t paddr_read(paddr_t addr, size_t len);
void paddr_write(paddr_t addr, size_t len, word_t data);

word_t vaddr_read(vaddr_t addr, size_t len);
void vaddr_write(vaddr_t write, size_t len, word_t data);
word_t vaddr_fetchinst(vaddr_t addr, size_t len);

paddr_t mmu_translate(vaddr_t vaddr, int type);

#define MMU_CHECK() (((CSR_Decode)csr(satp)).satp.MODE ? MMU_ON : MMU_OFF)

#endif