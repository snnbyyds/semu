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
