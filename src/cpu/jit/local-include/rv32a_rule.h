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

#ifndef __JIT_RV32A_RULE_H__
#define __JIT_RV32A_RULE_H__

#include <cpu/inst.h>
#include <jit.h>
#include "common_rule.h"

/* op_amo */
static inline void gen_lr_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_sc_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_amoswap_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
}

static inline void gen_amoadd_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amoxor_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amoand_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amoor_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amomin_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amomax_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amominu_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {

}

static inline void gen_amomaxu_w(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    
}

#endif
