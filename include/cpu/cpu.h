#ifndef __CPU_H__
#define __CPU_H__

#include <common.h>

#define NR_GPR 32
#define NR_CSR 4096

typedef struct {
    word_t csr[NR_CSR];
    word_t gpr[NR_GPR];
    vaddr_t pc;
} CPU_State;

void cpu_exec(uint64_t step);

extern CPU_State cpu;

#endif
