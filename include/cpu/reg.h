#ifndef __REG_H__
#define __REG_H__

#include <common.h>

#define gpr(i) cpu.gpr[i]
#define csr(i) cpu.csr[i]

enum {
    MSTATUS = 0x300,
    MTVEC   = 0x305,
    MEPC    = 0x341,
    MCAUSE  = 0x342
};

void reg_val_display();
char *reg_val_to_name(size_t idx);

#endif
