#ifndef __RV32F_H__
#define __RV32F_H__

#include <cpu/fpu.h>
#include "macros.h"

extern decode_t unimpl;

/* I-type */
static inline void op_load_fp(uint32_t inst, exec_t *info) {
    /* flw */
    F(rd).v = Mr_w(R(rs1) + IMM(I));
}

/* S-type */
static inline void op_store_fp(uint32_t inst, exec_t *info) {
    /* fsw */
    Mw_w(R(rs1) + IMM(S), F(rs2).v);
}

/* R4-type */
static inline void op_madd(uint32_t inst, exec_t *info) {
    /* fmadd.s */
    set_rounding_mode(decode_rm(inst));
    F(rd) = f32_mulAdd(F(rs1), F(rs2), F(rs3));
    set_fflag();
}

/* R4-type */
static inline void op_msub(uint32_t inst, exec_t *info) {
    /* fmsub.s */
    set_rounding_mode(decode_rm(inst));
    rv_float_t t = F(rs3);
    t.v ^= FLOAT_SIGN_MASK;
    F(rd) = f32_mulAdd(F(rs1), F(rs2), t);
    set_fflag();
}

/* R4-type */
static inline void op_nmsub(uint32_t inst, exec_t *info) {
    /* fnmsub.s */
    set_rounding_mode(decode_rm(inst));
    rv_float_t t = F(rs1);
    t.v ^= FLOAT_SIGN_MASK;
    F(rd) = f32_mulAdd(t, F(rs2), F(rs3));
    set_fflag();
}

/* R4-type */
static inline void op_nmadd(uint32_t inst, exec_t *info) {
    /* fnmadd.s */
    set_rounding_mode(decode_rm(inst));
    rv_float_t t1 = F(rs1);
    rv_float_t t3 = F(rs3);
    t1.v ^= FLOAT_SIGN_MASK;
    t3.v ^= FLOAT_SIGN_MASK;
    F(rd) = f32_mulAdd(t1, F(rs2), t3);
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
            F(__rd) = f32_add(F(__rs1), F(__rs2));
            set_fflag();
            break;
        case 0b0000100: /* fsub.s */
            set_rounding_mode(decode_rm(inst));
            F(__rd) = f32_sub(F(__rs1), F(__rs2));
            break;
        case 0b0001000: /* fmul.s */
            set_rounding_mode(decode_rm(inst));
            F(__rd) = f32_mul(F(__rs1), F(__rs2));
            set_fflag();
            break;
        case 0b0001100: /* fdiv.s */
            set_rounding_mode(decode_rm(inst));
            F(__rd) = f32_div(F(__rs1), F(__rs2));
            set_fflag();
            break;
        case 0b0101100: /* fsqrt.s */
            set_rounding_mode(decode_rm(inst));
            F(__rd) = f32_sqrt(F(__rs1));
            set_fflag();
            break;
        case 0b0010000:
            switch (decode_rm(inst)) {
                case 0b000: /* fsgnj.s */
                    F(__rd).v = (F(__rs1).v & ~FLOAT_SIGN_MASK) | (F(__rs2).v & FLOAT_SIGN_MASK);
                    break;
                case 0b001: /* fsgnjn.s */
                    F(__rd).v = (F(__rs1).v & ~FLOAT_SIGN_MASK) | (~F(__rs2).v & FLOAT_SIGN_MASK);
                    break;
                case 0b010: /* fsgnjx.s */
                    F(__rd).v = F(__rs1).v ^ (F(__rs2).v & FLOAT_SIGN_MASK);
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
                    R(__rd) = f32_to_i32(F(__rs1), softfloat_roundingMode, true);
                    set_fflag();
                    break;
                case 0b00001: /* fcvt.wu.s */
                    set_rounding_mode(decode_rm(inst));
                    R(__rd) = f32_to_ui32(F(__rs1), softfloat_roundingMode, true);
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
                    if (f32_isSignalingNaN(F(__rs1)) || f32_isSignalingNaN(F(__rs2))) {
                        cpu.csr_fcsr |= FCSR_NV_MASK;
                    }
                    const bool less = f32_lt_quiet(F(__rs1), F(__rs2)) ||
                                (f32_eq(F(__rs1), F(__rs2)) &&
                                (F(__rs1).v & FLOAT_SIGN_MASK));
                    if (is_nan(F(__rs1).v) && is_nan(F(__rs2).v))
                        F(__rd).v = 0x7fc00000; /* The canonical NaN */
                    else
                        F(__rd) = (less || is_nan(F(__rs2).v) ? F(__rs1) : F(__rs2));
                    break;
                case 0b001: /* fmax.s */
                    if (f32_isSignalingNaN(F(__rs1)) || f32_isSignalingNaN(F(__rs2))) {
                        cpu.csr_fcsr |= FCSR_NV_MASK;
                    }
                    const bool greater = f32_lt_quiet(F(__rs2), F(__rs1)) ||
                                (f32_eq(F(__rs1), F(__rs2)) &&
                                (F(__rs2).v & FLOAT_SIGN_MASK));
                    if (is_nan(F(__rs1).v) && is_nan(F(__rs2).v))
                        F(__rd).v = 0x7fc00000; /* The canonical NaN */
                    else
                        F(__rd) = (greater || is_nan(F(__rs2).v) ? F(__rs1) : F(__rs2));
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
                    R(__rd) = classify_s(F(__rs1).v);
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        case 0b1010000:
            switch (decode_rm(inst)) {
                case 0b010: /* feq.s */
                    R(__rd) = f32_eq(F(__rs1), F(__rs2));
                    set_fflag();
                    break;
                case 0b001: /* flt.s */
                    R(__rd) = f32_lt(F(__rs1), F(__rs2));
                    set_fflag();
                    break;
                case 0b000: /* fle.s */
                    R(__rd) = f32_le(F(__rs1), F(__rs2));
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
                    F(__rd) = i32_to_f32(R(__rs1));
                    set_fflag();
                    break;
                case 0b00001: /* fcvt.s.wu */
                    set_rounding_mode(decode_rm(inst));
                    F(__rd) = ui32_to_f32(R(__rs1));
                    set_fflag();
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        case 0b1111000: /* fmv.w.x */
            F(__rd).v = R(__rs1);
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

#endif
