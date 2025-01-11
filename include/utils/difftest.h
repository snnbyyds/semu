#ifndef __DIFFTEST_H__
#define __DIFFTEST_H__

#include <cpu/cpu.h>
#include <cpu/fpu.h>

#define DIFFTEST_OPS_INITIALIZER {NULL, NULL, NULL, NULL, NULL}

typedef struct {
    /* integer registers */
    uint32_t gpr[NR_GPR];
    uint32_t pc;

#ifdef CONFIG_DIFFTEST_REF_SPIKE
    /* float registers */
    fpr_t fpr[NR_FPR];
#endif
} riscv_context_t;

enum { DIFFTEST_TO_DUT = false, DIFFTEST_TO_REF = true };

void init_difftest(const char *ref_so, int port, size_t imgsize);
void difftest_skip_ref();
void difftest_exec_once();

#endif
