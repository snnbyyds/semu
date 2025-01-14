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

#ifndef __JIT_RV32FD_RULE_H__
#define __JIT_RV32FD_RULE_H__

#include <cpu/inst.h>
#include <jit.h>
#include "common_rule.h"

static inline void gen_flw(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fld(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsw(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsd(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmadd_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmadd_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmsub_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmsub_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fnmsub_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fnmsub_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fnmadd_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fnmadd_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fadd_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsub_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmul_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fdiv_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsqrt_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsgnj_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsgnjn_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsgnjx_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_w_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_wu_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmin_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmax_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmv_x_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fclass_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_feq_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_flt_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fle_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_s_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_s_wu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmv_w_x(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_s_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fadd_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsub_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmul_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fdiv_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsqrt_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsgnj_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsgnjn_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fsgnjx_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_w_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_wu_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fmin_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_fmax_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    
}

static inline void gen_fclass_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    
}

static inline void gen_feq_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_flt_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fle_d(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_d_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_d_wu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_fcvt_d_s(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

#endif
