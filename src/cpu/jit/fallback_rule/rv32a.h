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

#ifndef __EXEC_RV32A_H__
#define __EXEC_RV32A_H__

#include <cpu/fpu.h>
#include <cpu/inst.h>
#include "macros.h"

static inline void func_lr_w(exec_t *info, riscv_ir_t *ir) {
    R(rd) = Mr_w(R(rs1));
}

static inline void func_sc_w(exec_t *info, riscv_ir_t *ir) {
    Mw_w(R(rs1), R(rs2));
    R(rd) = 0;
}

static inline void func_amoswap_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), R(rs2));
    R(rd) = t;
}

static inline void func_amoadd_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), t + R(rs2));
    R(rd) = t;
}

static inline void func_amoxor_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), t ^ R(rs2));
    R(rd) = t;
}

static inline void func_amoand_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), t & R(rs2));
    R(rd) = t;
}

static inline void func_amoor_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), t | R(rs2));
    R(rd) = t;
}

static inline void func_amomin_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), (int32_t)t < (int32_t)R(rs2) ? t : R(rs2));
    R(rd) = t;
}

static inline void func_amomax_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), (int32_t)t > (int32_t)R(rs2) ? t : R(rs2));
    R(rd) = t;
}

static inline void func_amominu_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), t < R(rs2) ? t : R(rs2));
    R(rd) = t;
}

static inline void func_amomaxu_w(exec_t *info, riscv_ir_t *ir) {
    const uint32_t t = Mr_w(R(rs1));
    Mw_w(R(rs1), t > R(rs2) ? t : R(rs2));
    R(rd) = t;
}

#endif
