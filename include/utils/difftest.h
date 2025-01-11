/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
