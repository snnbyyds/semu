#ifndef __INST_MACROS_H__
#define __INST_MACROS_H__

#include <memory.h>
#include <cpu/reg.h>
#include <cpu/inst.h>

#define R(i) gpr(i)
#define F(i) fpr(i)

#define Mr_w vaddr_read_w
#define Mr_s vaddr_read_s
#define Mr_b vaddr_read_b
#define Mw_w vaddr_write_w
#define Mw_s vaddr_write_s
#define Mw_b vaddr_write_b

#define PC info->pc
#define NPC info->dnpc

#define rd decode_rd(inst)
#define rs1 decode_rs1(inst)
#define rs2 decode_rs2(inst)
#define rs3 decode_rs3(inst)

#define IMM(TYPE) decode_imm ## TYPE(inst)

#endif
