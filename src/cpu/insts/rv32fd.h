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

#ifndef __RV32FD_H__
#define __RV32FD_H__

#include <cpu/fpu.h>
#include "macros.h"

extern decode_t unimpl;

/* I-type */
static inline void op_load_fp(uint32_t inst, exec_t *info) {
    const uint32_t __rd = rd;
    switch (decode_rm(inst)) {
        case 0b010: /* flw */
            F(__rd).v = Mr_w(R(rs1) + IMM(I));
            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
            break;
        case 0b011: /* fld */
            F64(__rd).v = Mr_d(R(rs1) + IMM(I));
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* S-type */
static inline void op_store_fp(uint32_t inst, exec_t *info) {
    switch (decode_rm(inst)) {
        case 0b010: /* fsw */
            Mw_w(R(rs1) + IMM(S), F(rs2).v);
            break;
        case 0b011: /* fsd */
            Mw_d(R(rs1) + IMM(S), F64(rs2).v);
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* R4-type */
static inline void op_madd(uint32_t inst, exec_t *info) {
    const uint32_t __rd = rd;
    switch (decode_fmt(inst)) {
        case 0: /* fmadd.s */
            set_rounding_mode(decode_rm(inst));
            F32(__rd) = f32_mulAdd(unboxF32(F(rs1)), unboxF32(F(rs2)), unboxF32(F(rs3)));
            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
            set_fflag();
            break;
        case 1: /* fmadd.d */
            set_rounding_mode(decode_rm(inst));
            F64(__rd) = f64_mulAdd(F64(rs1), F64(rs2), F64(rs3));
            set_fflag();
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* R4-type */
static inline void op_msub(uint32_t inst, exec_t *info) {
    const uint32_t __rd = rd;
    switch (decode_fmt(inst)) {
        case 0: /* fmsub.s */
            set_rounding_mode(decode_rm(inst));
            float32_t t = unboxF32(F(rs3));
            t.v ^= FLOAT_SIGN_MASK;
            F32(__rd) = f32_mulAdd(unboxF32(F(rs1)), unboxF32(F(rs2)), t);
            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
            set_fflag();
            break;
        case 1: /* fmsub.d */
            set_rounding_mode(decode_rm(inst));
            float64_t _t = F64(rs3);
            _t.v ^= DOUBLE_SIGN_MASK;
            F64(__rd) = f64_mulAdd(F64(rs1), F64(rs2), _t);
            set_fflag();
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* R4-type */
static inline void op_nmsub(uint32_t inst, exec_t *info) {
    const uint32_t __rd = rd;
    switch (decode_fmt(inst)) {
        case 0: /* fnmsub.s */
            set_rounding_mode(decode_rm(inst));
            float32_t t = unboxF32(F(rs1));
            t.v ^= FLOAT_SIGN_MASK;
            F32(__rd) = f32_mulAdd(t, unboxF32(F(rs2)), unboxF32(F(rs3)));
            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
            set_fflag();
            break;
        case 1: /* fnmsub.d */
            set_rounding_mode(decode_rm(inst));
            float64_t _t = F64(rs1);
            _t.v ^= DOUBLE_SIGN_MASK;
            F64(__rd) = f64_mulAdd(_t, F64(rs2), F64(rs3));
            set_fflag();
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* R4-type */
static inline void op_nmadd(uint32_t inst, exec_t *info) {
    const uint32_t __rd = rd;
    switch (decode_fmt(inst)) {
        case 0: /* fnmadd.s */
            set_rounding_mode(decode_rm(inst));
            float32_t t1 = unboxF32(F(rs1));
            float32_t t3 = unboxF32(F(rs3));
            t1.v ^= FLOAT_SIGN_MASK;
            t3.v ^= FLOAT_SIGN_MASK;
            F32(__rd) = f32_mulAdd(t1, unboxF32(F(rs2)), t3);
            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
            set_fflag();
            break;
        case 1: /* fnmadd.d */
            set_rounding_mode(decode_rm(inst));
            float64_t _t1 = F64(rs1);
            float64_t _t3 = F64(rs3);
            _t1.v ^= DOUBLE_SIGN_MASK;
            _t3.v ^= DOUBLE_SIGN_MASK;
            F64(__rd) = f64_mulAdd(_t1, F64(rs2), _t3);
            set_fflag();
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* R-type */
static inline void op_op_fp(uint32_t inst, exec_t *info) {
    const uint32_t __rs1 = rs1;
    const uint32_t __rs2 = rs2;
    const uint32_t __rd = rd;
    switch (decode_fmt(inst)) {
        case 0:
            switch (decode_funct7(inst)) {
                case 0b0000000: /* fadd.s */
                    set_rounding_mode(decode_rm(inst));
                    F32(__rd) = f32_add(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    set_fflag();
                    break;
                case 0b0000100: /* fsub.s */
                    set_rounding_mode(decode_rm(inst));
                    F32(__rd) = f32_sub(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    set_fflag();
                    break;
                case 0b0001000: /* fmul.s */
                    set_rounding_mode(decode_rm(inst));
                    F32(__rd) = f32_mul(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    set_fflag();
                    break;
                case 0b0001100: /* fdiv.s */
                    set_rounding_mode(decode_rm(inst));
                    F32(__rd) = f32_div(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    set_fflag();
                    break;
                case 0b0101100: /* fsqrt.s */
                    set_rounding_mode(decode_rm(inst));
                    F32(__rd) = f32_sqrt(unboxF32(F(__rs1)));
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    set_fflag();
                    break;
                case 0b0010000:
                    switch (decode_rm(inst)) {
                        case 0b000: /* fsgnj.s */
                            F(__rd).v =
                                (unboxF32(F(__rs1)).v & ~FLOAT_SIGN_MASK) |
                                (unboxF32(F(__rs2)).v & FLOAT_SIGN_MASK);
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            break;
                        case 0b001: /* fsgnjn.s */
                            F(__rd).v =
                                (unboxF32(F(__rs1)).v & ~FLOAT_SIGN_MASK) |
                                (~unboxF32(F(__rs2)).v & FLOAT_SIGN_MASK);
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            break;
                        case 0b010: /* fsgnjx.s */
                            F(__rd).v =
                                unboxF32(F(__rs1)).v ^
                                (unboxF32(F(__rs2)).v & FLOAT_SIGN_MASK);
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1100000:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.w.s */
                            set_rounding_mode(decode_rm(inst));
                            R(__rd) = f32_to_i32(unboxF32(F(__rs1)), softfloat_roundingMode, true);
                            set_fflag();
                            break;
                        case 0b00001: /* fcvt.wu.s */
                            set_rounding_mode(decode_rm(inst));
                            R(__rd) = f32_to_ui32(unboxF32(F(__rs1)), softfloat_roundingMode, true);
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b0010100:
                    switch (decode_rm(inst)) {
                        case 0b000: /* fmin.s */
                            do {
                                const float32_t src1 = unboxF32(F(__rs1));
                                const float32_t src2 = unboxF32(F(__rs2));
                                if (f32_isSignalingNaN(src1) || f32_isSignalingNaN(src2)) {
                                    cpu.csr_fcsr |= FCSR_NV_MASK;
                                }
                                const bool less = f32_lt_quiet(src1, src2) ||
                                            (f32_eq(src1, src2) &&
                                            (src1.v & FLOAT_SIGN_MASK));
                                if (is_nan(src1.v) && is_nan(src2.v)) {
                                    F(__rd).v = 0x7fc00000; /* The canonical NaN */
                                } else {
                                    F32(__rd) = (less || is_nan(src2.v) ? src1 : src2);
                                }
                            } while(0);
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            break;
                        case 0b001: /* fmax.s */
                            do {
                                const float32_t src1 = unboxF32(F(__rs1));
                                const float32_t src2 = unboxF32(F(__rs2));
                                if (f32_isSignalingNaN(src1) || f32_isSignalingNaN(src2)) {
                                    cpu.csr_fcsr |= FCSR_NV_MASK;
                                }
                                const bool greater = f32_lt_quiet(src2, src1) ||
                                            (f32_eq(src1, src2) &&
                                            (src2.v & FLOAT_SIGN_MASK));
                                if (is_nan(src1.v) && is_nan(src2.v)) {
                                    F(__rd).v = 0x7fc00000; /* The canonical NaN */
                                } else {
                                    F32(__rd) = (greater || is_nan(F(__rs2).v) ? F32(__rs1) : F32(__rs2));
                                }
                            } while (0);
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1110000:
                    switch (decode_rm(inst)) {
                        case 0b000: /* fmv.x.w */
                            R(__rd) = F(__rs1).v;
                            break;
                        case 0b001: /* fclass.s */
                            R(__rd) = classify_s(unboxF32(F(__rs1)).v);
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1010000:
                    switch (decode_rm(inst)) {
                        case 0b010: /* feq.s */
                            R(__rd) = f32_eq(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                            set_fflag();
                            break;
                        case 0b001: /* flt.s */
                            R(__rd) = f32_lt(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                            set_fflag();
                            break;
                        case 0b000: /* fle.s */
                            R(__rd) = f32_le(unboxF32(F(__rs1)), unboxF32(F(__rs2)));
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1101000:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.s.w */
                            set_rounding_mode(decode_rm(inst));
                            F32(__rd) = i32_to_f32(R(__rs1));
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            set_fflag();
                            break;
                        case 0b00001: /* fcvt.s.wu */
                            set_rounding_mode(decode_rm(inst));
                            F32(__rd) = ui32_to_f32(R(__rs1));
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1111000: /* fmv.w.x */
                    F(__rd).v = R(__rs1);
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    break;
                case 0b0100000:
                    switch (__rs2) {
                        case 0b00001: /* fcvt.s.d */
                            F32(__rd) = f64_to_f32(F64(__rs1));
                            SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        case 1:
            switch (decode_funct7(inst)) {
                case 0b0000001: /* fadd.d */
                    set_rounding_mode(decode_rm(inst));
                    F64(__rd) = f64_add(F64(__rs1), F64(__rs2));
                    set_fflag();
                    break;
                case 0b0000101: /* fsub.d */
                    set_rounding_mode(decode_rm(inst));
                    F64(__rd) = f64_sub(F64(__rs1), F64(__rs2));
                    set_fflag();
                    break;
                case 0b0001001: /* fmul.d */
                    set_rounding_mode(decode_rm(inst));
                    F64(__rd) = f64_mul(F64(__rs1), F64(__rs2));
                    set_fflag();
                    break;
                case 0b0001101: /* fdiv.d */
                    set_rounding_mode(decode_rm(inst));
                    F64(__rd) = f64_div(F64(__rs1), F64(__rs2));
                    set_fflag();
                    break;
                case 0b0101101: /* fsqrt.d */
                    set_rounding_mode(decode_rm(inst));
                    F64(__rd) = f64_sqrt(F64(__rs1));
                    set_fflag();
                    break;
                case 0b0010001:
                    switch (decode_rm(inst)) {
                        case 0b000: /* fsgnj.d */
                            F64(__rd).v =
                                (F64(__rs1).v & ~DOUBLE_SIGN_MASK) |
                                (F64(__rs2).v & DOUBLE_SIGN_MASK);
                            break;
                        case 0b001: /* fsgnjn.d */
                            F64(__rd).v =
                                (F64(__rs1).v & ~DOUBLE_SIGN_MASK) |
                                (~F64(__rs2).v & DOUBLE_SIGN_MASK);
                            break;
                        case 0b010: /* fsgnjx.d */
                            F64(__rd).v =
                                F64(__rs1).v ^
                                (F64(__rs2).v & DOUBLE_SIGN_MASK);
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1100001:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.w.d */
                            set_rounding_mode(decode_rm(inst));
                            R(__rd) = f64_to_i32(F64(__rs1), softfloat_roundingMode, true);
                            set_fflag();
                            break;
                        case 0b00001: /* fcvt.wu.d */
                            set_rounding_mode(decode_rm(inst));
                            R(__rd) = f64_to_ui32(F64(__rs1), softfloat_roundingMode, true);
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b0010101:
                    switch (decode_rm(inst)) {
                        case 0b000: /* fmin.d */
                            if (f64_isSignalingNaN(F64(__rs1)) || f64_isSignalingNaN(F64(__rs2))) {
                                cpu.csr_fcsr |= FCSR_NV_MASK;
                            }
                            const bool less = f64_lt_quiet(F64(__rs1), F64(__rs2)) ||
                                            (f64_eq(F64(__rs1), F64(__rs2)) &&
                                            (F64(__rs1).v & DOUBLE_SIGN_MASK));
                            if (is_nan64(F64(__rs1).v) && is_nan64(F64(__rs2).v)) {
                                F64(__rd).v = UINT64_C(0x7FF8000000000000); /* The canonical NaN */
                            } else {
                                F64(__rd) = (less || is_nan64(F64(__rs2).v) ? F64(__rs1) : F64(__rs2));
                            }
                            break;
                        case 0b001: /* fmax.d */
                            if (f64_isSignalingNaN(F64(__rs1)) || f64_isSignalingNaN(F64(__rs2))) {
                                cpu.csr_fcsr |= FCSR_NV_MASK;
                            }
                            const bool greater = f64_lt_quiet(F64(__rs2), F64(__rs1)) ||
                                        (f64_eq(F64(__rs1), F64(__rs2)) &&
                                        (F64(__rs2).v & DOUBLE_SIGN_MASK));
                            if (is_nan64(F64(__rs1).v) && is_nan64(F64(__rs2).v)) {
                                F64(__rd).v = UINT64_C(0x7FF8000000000000); /* The canonical NaN */
                            } else {
                                F64(__rd) = (greater || is_nan64(F64(__rs2).v) ? F64(__rs1) : F64(__rs2));
                            }
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1110001:
                    switch (decode_rm(inst)) {
                        case 0b001: /* fclass.d */
                            R(__rd) = classify_d(F64(__rs1).v);
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1010001:
                    switch (decode_rm(inst)) {
                        case 0b010: /* feq.d */
                            R(__rd) = f64_eq(F64(__rs1), F64(__rs2));
                            set_fflag();
                            break;
                        case 0b001: /* flt.d */
                            R(__rd) = f64_lt(F64(__rs1), F64(__rs2));
                            set_fflag();
                            break;
                        case 0b000: /* fle.d */
                            R(__rd) = f64_le(F64(__rs1), F64(__rs2));
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b1101001:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.d.w */
                            set_rounding_mode(decode_rm(inst));
                            F64(__rd) = i32_to_f64(R(__rs1));
                            set_fflag();
                            break;
                        case 0b00001: /* fcvt.d.wu */
                            set_rounding_mode(decode_rm(inst));
                            F64(__rd) = ui32_to_f64(R(__rs1));
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                case 0b0100001:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.d.s */
                            set_rounding_mode(decode_rm(inst));
                            F64(__rd) = f32_to_f64(unboxF32(F(__rs1)));
                            set_fflag();
                            break;
                        default:
                            unimpl(inst, info);
                            break;
                    }
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

#endif
