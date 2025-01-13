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

#ifndef __CPU_H__
#define __CPU_H__

#include <common.h>

#define NR_GPR 32
#define NR_FPR 32
#define NR_CSR 4096
#define FLEN 64

typedef union {
    float64_t _64;
    uint64_t v64;
    float32_t _32;
    uint32_t v;
} fpr_t;

typedef struct {
    /* integer registers */
    word_t gpr[NR_GPR];
    vaddr_t pc;

    /* float registers */
    fpr_t fpr[NR_FPR];

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

extern CPU_State cpu;

#endif
