#ifndef __RV32FD_H__
#define __RV32FD_H__

#include <cpu/fpu.h>
#include "macros.h"

extern decode_t unimpl;

/* I-type */
static inline void op_load_fp(uint32_t inst, exec_t *info) {
    /* flw */
    const uint32_t __rd = rd;
    F(__rd).v = Mr_w(R(rs1) + IMM(I));
    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
}

/* S-type */
static inline void op_store_fp(uint32_t inst, exec_t *info) {
    /* fsw */
    Mw_w(R(rs1) + IMM(S), F(rs2).v);
}

/* R4-type */
static inline void op_madd(uint32_t inst, exec_t *info) {
    /* fmadd.s */
    const uint32_t __rd = rd;
    set_rounding_mode(decode_rm(inst));
    F32(__rd) = f32_mulAdd(unboxF32(F(rs1)), unboxF32(F(rs2)), unboxF32(F(rs3)));
    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
    set_fflag();
}

/* R4-type */
static inline void op_msub(uint32_t inst, exec_t *info) {
    /* fmsub.s */
    const uint32_t __rd = rd;
    set_rounding_mode(decode_rm(inst));
    float32_t t = unboxF32(F(rs3));
    t.v ^= FLOAT_SIGN_MASK;
    F32(__rd) = f32_mulAdd(unboxF32(F(rs1)), unboxF32(F(rs2)), t);
    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
    set_fflag();
}

/* R4-type */
static inline void op_nmsub(uint32_t inst, exec_t *info) {
    /* fnmsub.s */
    const uint32_t __rd = rd;
    set_rounding_mode(decode_rm(inst));
    float32_t t = unboxF32(F(rs1));
    t.v ^= FLOAT_SIGN_MASK;
    F32(__rd) = f32_mulAdd(t, unboxF32(F(rs2)), unboxF32(F(rs3)));
    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
    set_fflag();
}

/* R4-type */
static inline void op_nmadd(uint32_t inst, exec_t *info) {
    /* fnmadd.s */
    const uint32_t __rd = rd;
    set_rounding_mode(decode_rm(inst));
    float32_t t1 = unboxF32(F(rs1));
    float32_t t3 = unboxF32(F(rs3));
    t1.v ^= FLOAT_SIGN_MASK;
    t3.v ^= FLOAT_SIGN_MASK;
    F32(__rd) = f32_mulAdd(t1, unboxF32(F(rs2)), t3);
    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
    set_fflag();
}

/* R-type */
static inline void op_op_fp(uint32_t inst, exec_t *info) {
    const uint32_t __rs1 = rs1;
    const uint32_t __rs2 = rs2;
    const uint32_t __rd = rd;
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
                    F(__rd).v = (unboxF32(F(__rs1)).v & ~FLOAT_SIGN_MASK) | (unboxF32(F(__rs2)).v & FLOAT_SIGN_MASK);
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    break;
                case 0b001: /* fsgnjn.s */
                    F(__rd).v = (unboxF32(F(__rs1)).v & ~FLOAT_SIGN_MASK) | (~unboxF32(F(__rs2)).v & FLOAT_SIGN_MASK);
                    SET_FPR_NAN_BOX_UPPER_BITS(__rd);
                    break;
                case 0b010: /* fsgnjx.s */
                    F(__rd).v = unboxF32(F(__rs1)).v ^ (unboxF32(F(__rs2)).v & FLOAT_SIGN_MASK);
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
        default:
            unimpl(inst, info);
            break;
    }
}

#endif
