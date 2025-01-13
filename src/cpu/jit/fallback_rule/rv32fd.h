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

#ifndef __EXEC_RV32FD_H__
#define __EXEC_RV32FD_H__

#include <cpu/fpu.h>
#include <cpu/inst.h>
#include "macros.h"

static inline void func_flw(exec_t *info, riscv_ir_t *ir) {
    F(rd).v = Mr_w(R(rs1) + imm);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fld(exec_t *info, riscv_ir_t *ir) {
    F64(rd).v = Mr_d(R(rs1) + imm);
}

static inline void func_fsw(exec_t *info, riscv_ir_t *ir) {
    Mw_w(R(rs1) + imm, F(rs2).v);
}

static inline void func_fsd(exec_t *info, riscv_ir_t *ir) {
    Mw_d(R(rs1) + imm, F64(rs2).v);
}

static inline void func_fmadd_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = f32_mulAdd(unboxF32(F(rs1)), unboxF32(F(rs2)), unboxF32(F(rs3)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fmadd_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f64_mulAdd(F64(rs1), F64(rs2), F64(rs3));
    set_fflag();
}

static inline void func_fmsub_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    float32_t t = unboxF32(F(rs3));
    t.v ^= FLOAT_SIGN_MASK;
    F32(rd) = f32_mulAdd(unboxF32(F(rs1)), unboxF32(F(rs2)), t);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fmsub_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    float64_t t = F64(rs3);
    t.v ^= DOUBLE_SIGN_MASK;
    F64(rd) = f64_mulAdd(F64(rs1), F64(rs2), t);
    set_fflag();
}

static inline void func_fnmsub_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    float32_t t1 = unboxF32(F(rs1));
    t1.v ^= FLOAT_SIGN_MASK;
    F32(rd) = f32_mulAdd(t1, unboxF32(F(rs2)), unboxF32(F(rs3)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fnmsub_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    float64_t t1 = F64(rs1);
    t1.v ^= DOUBLE_SIGN_MASK;
    F64(rd) = f64_mulAdd(t1, F64(rs2), F64(rs3));
    set_fflag();
}

static inline void func_fnmadd_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    float32_t t1 = unboxF32(F(rs1));
    float32_t t3 = unboxF32(F(rs3));
    t1.v ^= FLOAT_SIGN_MASK;
    t3.v ^= FLOAT_SIGN_MASK;
    F32(rd) = f32_mulAdd(t1, unboxF32(F(rs2)), t3);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fnmadd_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    float64_t t1 = F64(rs1);
    float64_t t3 = F64(rs3);
    t1.v ^= DOUBLE_SIGN_MASK;
    t3.v ^= DOUBLE_SIGN_MASK;
    F64(rd) = f64_mulAdd(t1, F64(rs2), t3);
    set_fflag();
}

static inline void func_fadd_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = f32_add(unboxF32(F(rs1)), unboxF32(F(rs2)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fsub_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = f32_sub(unboxF32(F(rs1)), unboxF32(F(rs2)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fmul_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = f32_mul(unboxF32(F(rs1)), unboxF32(F(rs2)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_div_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = f32_div(unboxF32(F(rs1)), unboxF32(F(rs2)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fsqrt_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = f32_sqrt(unboxF32(F(rs1)));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fsgnj_s(exec_t *info, riscv_ir_t *ir) {
    F(rd).v =
        (unboxF32(F(rs1)).v & ~FLOAT_SIGN_MASK) |
        (unboxF32(F(rs2)).v & FLOAT_SIGN_MASK);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fsgnjn_s(exec_t *info, riscv_ir_t *ir) {
    F(rd).v =
        (unboxF32(F(rs1)).v & ~FLOAT_SIGN_MASK) |
        (~unboxF32(F(rs2)).v & FLOAT_SIGN_MASK);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fsgnjx_s(exec_t *info, riscv_ir_t *ir) {
    F(rd).v =
        unboxF32(F(rs1)).v ^
        (unboxF32(F(rs2)).v & FLOAT_SIGN_MASK);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fcvt_w_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    R(rd) = f32_to_i32(unboxF32(F(rs1)), softfloat_roundingMode, true);
    set_fflag();
}

static inline void func_fcvt_wu_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    R(rd) = f32_to_ui32(unboxF32(F(rs1)), softfloat_roundingMode, true);
    set_fflag();
}

static inline void func_fmin_s(exec_t *info, riscv_ir_t *ir) {
    const float32_t src1 = unboxF32(F(rs1));
    const float32_t src2 = unboxF32(F(rs2));
    if (f32_isSignalingNaN(src1) || f32_isSignalingNaN(src2)) {
        cpu.csr_fcsr |= FCSR_NV_MASK;
    }
    const bool less = f32_lt_quiet(src1, src2) ||
                (f32_eq(src1, src2) &&
                (src1.v & FLOAT_SIGN_MASK));
    if (is_nan(src1.v) && is_nan(src2.v)) {
        F(rd).v = 0x7fc00000; /* The canonical NaN */
    } else {
        F32(rd) = (less || is_nan(src2.v) ? src1 : src2);
    }
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fmax_s(exec_t *info, riscv_ir_t *ir) {
    const float32_t src1 = unboxF32(F(rs1));
    const float32_t src2 = unboxF32(F(rs2));
    if (f32_isSignalingNaN(src1) || f32_isSignalingNaN(src2)) {
        cpu.csr_fcsr |= FCSR_NV_MASK;
    }
    const bool greater = f32_lt_quiet(src2, src1) ||
                (f32_eq(src1, src2) &&
                (src2.v & FLOAT_SIGN_MASK));
    if (is_nan(src1.v) && is_nan(src2.v)) {
        F(rd).v = 0x7fc00000; /* The canonical NaN */
    } else {
        F32(rd) = (greater || is_nan(F(rs2).v) ? F32(rs1) : F32(rs2));
    }
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fmv_x_w(exec_t *info, riscv_ir_t *ir) {
    R(rd) = F(rs1).v;
}

static inline void func_fclass_s(exec_t *info, riscv_ir_t *ir) {
    R(rd) = classify_s(unboxF32(F(rs1)).v);
}

static inline void func_feq_s(exec_t *info, riscv_ir_t *ir) {
    R(rd) = f32_eq(unboxF32(F(rs1)), unboxF32(F(rs2)));
    set_fflag();
}

static inline void func_flt_s(exec_t *info, riscv_ir_t *ir) {
    R(rd) = f32_lt(unboxF32(F(rs1)), unboxF32(F(rs2)));
    set_fflag();
}

static inline void func_fle_s(exec_t *info, riscv_ir_t *ir) {
    R(rd) = f32_le(unboxF32(F(rs1)), unboxF32(F(rs2)));
    set_fflag();
}

static inline void func_fcvt_s_w(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = i32_to_f32(R(rs1));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fcvt_s_wu(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F32(rd) = ui32_to_f32(R(rs1));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
    set_fflag();
}

static inline void func_fmv_w_x(exec_t *info, riscv_ir_t *ir) {
    F(rd).v = R(rs1);
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fcvt_s_d(exec_t *info, riscv_ir_t *ir) {
    F32(rd) = f64_to_f32(F64(rs1));
    SET_FPR_NAN_BOX_UPPER_BITS(rd);
}

static inline void func_fadd_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f64_add(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_fsub_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f64_sub(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_fmul_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f64_mul(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_fdiv_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f64_div(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_fsqrt_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f64_sqrt(F64(rs1));
    set_fflag();
}

static inline void func_fsgnj_d(exec_t *info, riscv_ir_t *ir) {
    F64(rd).v =
        (F64(rs1).v & ~DOUBLE_SIGN_MASK) |
        (F64(rs2).v & DOUBLE_SIGN_MASK);
}

static inline void func_fsgnjn_d(exec_t *info, riscv_ir_t *ir) {
    F64(rd).v =
        (F64(rs1).v & ~DOUBLE_SIGN_MASK) |
        (~F64(rs2).v & DOUBLE_SIGN_MASK);
}

static inline void func_fsgnjx_d(exec_t *info, riscv_ir_t *ir) {
    F64(rd).v =
        F64(rs1).v ^
        (F64(rs2).v & DOUBLE_SIGN_MASK);
}

static inline void func_fcvt_w_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    R(rd) = f64_to_i32(F64(rs1), softfloat_roundingMode, true);
    set_fflag();
}

static inline void func_fcvt_wu_d(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    R(rd) = f64_to_ui32(F64(rs1), softfloat_roundingMode, true);
    set_fflag();
}

static inline void func_fmin_d(exec_t *info, riscv_ir_t *ir) {
    if (f64_isSignalingNaN(F64(rs1)) || f64_isSignalingNaN(F64(rs2))) {
        cpu.csr_fcsr |= FCSR_NV_MASK;
    }
    const bool less = f64_lt_quiet(F64(rs1), F64(rs2)) ||
                    (f64_eq(F64(rs1), F64(rs2)) &&
                    (F64(rs1).v & DOUBLE_SIGN_MASK));
    if (is_nan64(F64(rs1).v) && is_nan64(F64(rs2).v)) {
        F64(rd).v = UINT64_C(0x7FF8000000000000); /* The canonical NaN */
    } else {
        F64(rd) = (less || is_nan64(F64(rs2).v) ? F64(rs1) : F64(rs2));
    }
}

static inline void func_fmax_d(exec_t *info, riscv_ir_t *ir) {
    if (f64_isSignalingNaN(F64(rs1)) || f64_isSignalingNaN(F64(rs2))) {
        cpu.csr_fcsr |= FCSR_NV_MASK;
    }
    const bool greater = f64_lt_quiet(F64(rs2), F64(rs1)) ||
                (f64_eq(F64(rs1), F64(rs2)) &&
                (F64(rs2).v & DOUBLE_SIGN_MASK));
    if (is_nan64(F64(rs1).v) && is_nan64(F64(rs2).v)) {
        F64(rd).v = UINT64_C(0x7FF8000000000000); /* The canonical NaN */
    } else {
        F64(rd) = (greater || is_nan64(F64(rs2).v) ? F64(rs1) : F64(rs2));
    }
}

static inline void func_fclass_d(exec_t *info, riscv_ir_t *ir) {
    R(rd) = classify_d(F64(rs1).v);
}

static inline void func_feq_d(exec_t *info, riscv_ir_t *ir) {
    R(rd) = f64_eq(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_flt_d(exec_t *info, riscv_ir_t *ir) {
    R(rd) = f64_lt(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_fle_d(exec_t *info, riscv_ir_t *ir) {
    R(rd) = f64_le(F64(rs1), F64(rs2));
    set_fflag();
}

static inline void func_fcvt_d_w(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = i32_to_f64(R(rs1));
    set_fflag();
}

static inline void func_fcvt_d_wu(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = ui32_to_f64(R(rs1));
    set_fflag();
}

static inline void func_fcvt_d_s(exec_t *info, riscv_ir_t *ir) {
    set_rounding_mode(rm);
    F64(rd) = f32_to_f64(unboxF32(F(rs1)));
    set_fflag();
}

#endif
