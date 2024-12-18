#ifndef __CPU_H__
#define __CPU_H__

#include <common.h>

#define IRQ_TIMER 0x80000007
#define INTR_EMPTY -1

typedef struct {
    word_t gpr[32];
    word_t csr[4096];
    vaddr_t pc;
    bool intr;
} CPU_State;

extern CPU_State cpu;

void cpu_exec(uint64_t step);
word_t raise_intr(word_t NO, vaddr_t epc);

#endif
