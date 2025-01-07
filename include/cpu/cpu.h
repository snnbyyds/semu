#ifndef __CPU_H__
#define __CPU_H__

#include <common.h>

#define NR_GPR 32
#define NR_CSR 4096

typedef struct {
    /* gprs and pc */
    word_t gpr[NR_GPR];
    vaddr_t pc;

    /* csr registers */
    word_t csr_satp;     // 0x180
    word_t csr_mstatus;  // 0x300
    word_t csr_mtvec;    // 0x305
    word_t csr_mscratch; // 0x340
    word_t csr_mepc;     // 0x341
    word_t csr_mcause;   // 0x342

    /* intr flag */
    bool intr;
} CPU_State;

void cpu_exec(uint64_t step);

extern CPU_State cpu;

#endif
