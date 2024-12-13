#ifndef __CPU_H__
#define __CPU_H__

#include <common.h>

typedef struct {
    word_t gpr[32];
    word_t csr[4096];
    vaddr_t pc;
} CPU_State;

void cpu_exec(uint64_t step);

extern CPU_State cpu;

#endif
