#ifndef __CPU_H__
#define __CPU_H__

#include "reg.h"

#define IRQ_TIMER 0x80000007
#define INTR_EMPTY -1

#define RAISE_INTR(NO, PC) \
    ({ \
        WRITE_CSR(mstatus, MPIE, READ_CSR(mstatus, MIE)); \
        WRITE_CSR(mstatus, MIE, 0); \
        csr(mepc) = (PC); \
        csr(mcause) = (NO); \
        csr(mtvec); \
    })

#define QUERY_INTR() \
    ( \
        (READ_CSR(mstatus, MIE) && cpu.intr) ? (cpu.intr = false, IRQ_TIMER) : INTR_EMPTY \
    )

typedef struct {
    word_t gpr[32];
    word_t csr[4096];
    vaddr_t pc;
    bool intr;
} CPU_State;

void cpu_exec(uint64_t step);

extern CPU_State cpu;

#endif
