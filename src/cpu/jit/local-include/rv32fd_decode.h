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

#ifndef __JIT_RV32FD_DECODE_H__
#define __JIT_RV32FD_DECODE_H__

#include <cpu/inst.h>

/* I-type */
static inline riscv_inst_t decode_op_load_fp(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->imm = decode_immI(inst);
    switch (decode_rm(inst)) {
        case 0b010: /* flw */
            return inst_flw;
        case 0b011: /* fld */
            return inst_fld;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* S-type */
static inline riscv_inst_t decode_op_store_fp(uint32_t inst, riscv_ir_t *ir) {
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->imm = decode_immS(inst);
    switch (decode_rm(inst)) {
        case 0b010: /* fsw */
            return inst_fsw;
        case 0b011: /* fsd */
            return inst_fsd;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* R4-type */
static inline riscv_inst_t decode_op_madd(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->rs3 = decode_rs3(inst);
    ir->rm = decode_rm(inst);
    switch (decode_fmt(inst)) {
        case 0: /* fmadd.s */
            return inst_fmadd_s;
        case 1: /* fmadd.d */
            return inst_fmadd_d;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* R4-type */
static inline riscv_inst_t decode_op_msub(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->rs3 = decode_rs3(inst);
    ir->rm = decode_rm(inst);
    switch (decode_fmt(inst)) {
        case 0: /* fmsub.s */
            return inst_fmsub_s;
        case 1: /* fmsub.d */
            return inst_fmsub_d;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* R4-type */
static inline riscv_inst_t decode_op_nmsub(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->rs3 = decode_rs3(inst);
    ir->rm = decode_rm(inst);
    switch (decode_fmt(inst)) {
        case 0: /* fnmsub.s */
            return inst_fnmsub_s;
        case 1: /* fnmsub.d */
            return inst_fnmsub_d;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* R4-type */
static inline riscv_inst_t decode_op_nmadd(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->rs3 = decode_rs3(inst);
    ir->rm = decode_rm(inst);
    switch (decode_fmt(inst)) {
        case 0: /* fnmadd.s */
            return inst_fnmadd_s;
        case 1: /* fnmadd.d */
            return inst_fnmadd_d;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* R-type */
static inline riscv_inst_t decode_op_op_fp(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    const uint32_t __rs2 = (ir->rs2 = decode_rs2(inst));
    const uint32_t __rm = (ir->rm = decode_rm(inst));
    switch (decode_fmt(inst)) {
        case 0:
            switch (decode_funct7(inst)) {
                case 0b0000000: /* fadd.s */
                    return inst_fadd_s;
                case 0b0000100: /* fsub.s */
                    return inst_fsub_s;
                case 0b0001000: /* fmul.s */
                    return inst_fmul_s;
                case 0b0001100: /* fdiv.s */
                    return inst_fdiv_s;
                case 0b0101100: /* fsqrt.s */
                    return inst_fsqrt_s;
                case 0b0010000:
                    switch (__rm) {
                        case 0b000: /* fsgnj.s */
                            return inst_fsgnj_s;
                        case 0b001: /* fsgnjn.s */
                            return inst_fsgnjn_s;
                        case 0b010: /* fsgnjx.s */
                            return inst_fsgnjx_s;
                        default:
                            return inst_err;
                    }
                case 0b1100000:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.w.s */
                            return inst_fcvt_w_s;
                        case 0b00001: /* fcvt.wu.s */
                            return inst_fcvt_wu_s;
                        default:
                            return inst_err;
                    }
                case 0b0010100:
                    switch (__rm) {
                        case 0b000: /* fmin.s */
                            return inst_fmin_s;
                        case 0b001: /* fmax.s */
                            return inst_fmax_s;
                        default:
                            return inst_err;
                    }
                case 0b1110000:
                    switch (__rm) {
                        case 0b000: /* fmv.x.w */
                            return inst_fmv_x_w;
                        case 0b001: /* fclass.s */
                            return inst_fclass_s;
                        default:
                            return inst_err;
                    }
                case 0b1010000:
                    switch (__rm) {
                        case 0b010: /* feq.s */
                            return inst_feq_s;
                        case 0b001: /* flt.s */
                            return inst_flt_s;
                        case 0b000: /* fle.s */
                            return inst_fle_s;
                        default:
                            return inst_err;
                    }
                case 0b1101000:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.s.w */
                            return inst_fcvt_s_w;
                        case 0b00001: /* fcvt.s.wu */
                            return inst_fcvt_s_wu;
                        default:
                            return inst_err;
                    }
                case 0b1111000: /* fmv.w.x */
                    return inst_fmv_w_x;
                case 0b0100000:
                    switch (__rs2) {
                        case 0b00001: /* fcvt.s.d */
                            return inst_fcvt_s_d;
                        default:
                            return inst_err;
                    }
                default:
                    return inst_err;
            }
        case 1:
            switch (decode_funct7(inst)) {
                case 0b0000001: /* fadd.d */
                    return inst_fadd_d;
                case 0b0000101: /* fsub.d */
                    return inst_fsub_d;
                case 0b0001001: /* fmul.d */
                    return inst_fmul_d;
                case 0b0001101: /* fdiv.d */
                    return inst_fdiv_d;
                case 0b0101101: /* fsqrt.d */
                    return inst_fsqrt_d;
                case 0b0010001:
                    switch (__rm) {
                        case 0b000: /* fsgnj.d */
                            return inst_fsgnj_d;
                        case 0b001: /* fsgnjn.d */
                            return inst_fsgnjn_d;
                        case 0b010: /* fsgnjx.d */
                            return inst_fsgnjx_d;
                        default:
                            return inst_err;
                    }
                case 0b1100001:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.w.d */
                            return inst_fcvt_w_d;
                        case 0b00001: /* fcvt.wu.d */
                            return inst_fcvt_wu_d;
                        default:
                            return inst_err;
                    }
                case 0b0010101:
                    switch (__rm) {
                        case 0b000: /* fmin.d */
                            return inst_fmin_d;
                        case 0b001: /* fmax.d */
                            return inst_fmax_d;
                        default:
                            return inst_err;
                    }
                    break;
                case 0b1110001:
                    switch (__rm) {
                        case 0b001: /* fclass.d */
                            return inst_fclass_d;
                        default:
                            return inst_err;
                    }
                case 0b1010001:
                    switch (__rm) {
                        case 0b010: /* feq.d */
                            return inst_feq_d;
                        case 0b001: /* flt.d */
                            return inst_flt_d;
                        case 0b000: /* fle.d */
                            return inst_fle_d;
                        default:
                            return inst_err;
                    }
                case 0b1101001:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.d.w */
                            return inst_fcvt_d_w;
                        case 0b00001: /* fcvt.d.wu */
                            return inst_fcvt_d_wu;
                        default:
                            return inst_err;
                    }
                case 0b0100001:
                    switch (__rs2) {
                        case 0b00000: /* fcvt.d.s */
                            return inst_fcvt_d_s;
                        default:
                            return inst_err;
                    }
                default:
                    return inst_err;
            }
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

#endif
