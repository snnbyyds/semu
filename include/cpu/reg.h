#ifndef __REG_H__
#define __REG_H__

#include <common.h>

#define gpr(i) cpu.gpr[i]
#define csr(i) cpu.csr[i]

void reg_val_display();

#endif
