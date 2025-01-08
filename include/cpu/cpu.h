#ifndef __CPU_H__
#define __CPU_H__

#include <common.h>

#define NR_GPR 32
#define NR_FPR 32
#define NR_CSR 4096
#define FLEN 32

typedef struct {
    /* integer registers */
    word_t gpr[NR_GPR];
    vaddr_t pc;

    /* float registers */
    rv_float_t fpr[NR_FPR];

    /* csr registers */
    uint32_t csr_fcsr;     // 0x003

    uint32_t csr_satp;     // 0x180

    uint32_t csr_mstatus;  // 0x300
    uint32_t csr_mtvec;    // 0x305
    uint32_t csr_mscratch; // 0x340
    uint32_t csr_mepc;     // 0x341
    uint32_t csr_mcause;   // 0x342

    /* intr flag */
    bool intr;
} CPU_State;

void cpu_exec(uint64_t step);

extern CPU_State cpu;

#endif
