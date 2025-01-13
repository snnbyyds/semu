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

#ifndef __EXEC_MACROS_H__
#define __EXEC_MACROS_H__

#include <memory.h>
#include <cpu/fpu.h>
#include <cpu/inst.h>

#define R(i) gpr(i)
#define F(i) fpr(i)
#define F32(i) ACCESS_F32(i)
#define F64(i) ACCESS_F64(i)

#define Mr_d vaddr_read_d
#define Mr_w vaddr_read_w
#define Mr_s vaddr_read_s
#define Mr_b vaddr_read_b
#define Mw_d vaddr_write_d
#define Mw_w vaddr_write_w
#define Mw_s vaddr_write_s
#define Mw_b vaddr_write_b

#define PC info->pc
#define NPC info->dnpc

#define rd (ir->rd)
#define rs1 (ir->rs1)
#define rs2 (ir->rs2)
#define rs3 (ir->rs3)
#define imm (ir->imm)
#define rm (ir->rm)

#endif
