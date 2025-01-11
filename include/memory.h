#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <common.h>

#define PTE_V_SHIFT  0
#define PTE_R_SHIFT  1
#define PTE_W_SHIFT  2
#define PTE_X_SHIFT  3
#define PTE_U_SHIFT  4
#define PTE_G_SHIFT  5
#define PTE_A_SHIFT  6
#define PTE_D_SHIFT  7
#define PTE_RSW_SHIFT 8
#define PTE_PPN_SHIFT 10
#define PTE_V        (1 << PTE_V_SHIFT)
#define PTE_R        (1 << PTE_R_SHIFT)
#define PTE_W        (1 << PTE_W_SHIFT)
#define PTE_X        (1 << PTE_X_SHIFT)
#define PTE_U        (1 << PTE_U_SHIFT)
#define PTE_G        (1 << PTE_G_SHIFT)
#define PTE_A        (1 << PTE_A_SHIFT)
#define PTE_D        (1 << PTE_D_SHIFT)
#define PTE_RSW      (3 << PTE_RSW_SHIFT)
#define PTE_PPN      (0x3FFFFF << PTE_PPN_SHIFT) // PPN has 22 bits

#define VA_OFFSET_SHIFT  0
#define VA_VPN_0_SHIFT   12
#define VA_VPN_1_SHIFT   22
#define VA_OFFSET        (0xFFF << VA_OFFSET_SHIFT) // 12 bits for offset
#define VA_VPN_0         (0x3FF << VA_VPN_0_SHIFT)  // 10 bits for VPN_0
#define VA_VPN_1         (0x3FF << VA_VPN_1_SHIFT)  // 10 bits for VPN_1

#define PA_OFFSET_SHIFT  0
#define PA_PPN_SHIFT     12
#define PA_OFFSET        (0xFFF << PA_OFFSET_SHIFT) // 12 bits for offset
#define PA_PPN           (0xFFFFF << PA_PPN_SHIFT)  // 20 bits for PPN (not 22 for now...)

#define IN_PMEM(ADDR) \
    ((ADDR) >= CONFIG_MBASE && (ADDR) < CONFIG_MBASE + CONFIG_MSIZE)

typedef word_t PTE;
typedef enum { MMU_DIRECT, MMU_TRANSLATE, MMU_INVALID } mmu_state_t;
typedef enum { MEM_TYPE_READ, MEM_TYPE_WRITE, MEM_TYPE_IFETCH } mem_access_t;

/* ptr to the memory array */
extern void *pmem;

#define GUEST_TO_HOST(PADDR) \
    ((void *)(pmem + ((paddr_t)(PADDR) - CONFIG_MBASE)))

#define HOST_TO_GUEST(HADDR) \
    ((paddr_t)((const void *)(HADDR) - (const void *)pmem + CONFIG_MBASE))

/* Read from host address */
#define HOST_READ(HADDR, LEN) \
    ({ \
        word_t __ret = -1; \
        switch (len) { \
            case 1: __ret = *(uint8_t *)(HADDR); break; \
            case 2: __ret = *(uint16_t *)(HADDR); break; \
            case 4: __ret = *(uint32_t *)(HADDR); break; \
            case 8: __ret = *(uint64_t *)(HADDR); break; \
            default: assert(0); \
        } \
        __ret; \
    })

/* Write to host address */
#define HOST_WRITE(HADDR, LEN, DATA) \
    do { \
        switch (LEN) { \
            case 1: *(uint8_t *)(HADDR) = (uint8_t)(DATA); break; \
            case 2: *(uint16_t *)(HADDR) = (uint16_t)(DATA); break; \
            case 4: *(uint32_t *)(HADDR) = (uint32_t)(DATA); break; \
            case 8: *(uint64_t *)(HADDR) = (uint64_t)(DATA); break; \
            default: assert(0); \
        } \
    } while (0)

uint64_t vaddr_read_d(vaddr_t addr);
uint32_t vaddr_read_w(vaddr_t addr);
uint16_t vaddr_read_s(vaddr_t addr);
uint8_t vaddr_read_b(vaddr_t addr);

void vaddr_write_d(vaddr_t addr, uint64_t data);
void vaddr_write_w(vaddr_t addr, uint32_t data);
void vaddr_write_s(vaddr_t addr, uint16_t data);
void vaddr_write_b(vaddr_t addr, uint8_t data);

word_t vaddr_ifetch(vaddr_t addr);

#endif