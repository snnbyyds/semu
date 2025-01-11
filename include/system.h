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

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "cpu/reg.h"
#include "device/timer.h"

void etrace(word_t no, vaddr_t epc);

#define ISA_QUERY_INTR() \
    ((cpu.intr && ((cpu.csr_mstatus) & MSTATUS_MIE)) ? \
    ((cpu.intr = false), IRQ_TIMER) : -1)

#define ISA_RAISE_INTR(NO, EPC) \
    ({ \
        etrace((NO), (EPC)); \
        const uint32_t mstatus_mie = \
            (cpu.csr_mstatus & MSTATUS_MIE) >> MSTATUS_MIE_SHIFT; \
        cpu.csr_mstatus |= (mstatus_mie << MSTATUS_MPIE_SHIFT); \
        cpu.csr_mstatus &= ~MSTATUS_MIE; \
        cpu.csr_mepc = (EPC); \
        cpu.csr_mcause = (NO); \
        cpu.csr_mtvec; \
    })

#endif
