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

#ifndef __JIT_RV32A_DECODE_H__
#define __JIT_RV32A_DECODE_H__

#include <cpu/inst.h>

/* R-type */
static inline riscv_inst_t decode_op_amo(uint32_t inst, riscv_ir_t *ir) {
    const uint32_t funct5 = (decode_funct7(inst) >> 2) & 0x1f;
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    switch (funct5) {
        case 0b00010: /* lr.w */
            return inst_lr_w;
        case 0b00011: /* sc.w */
            return inst_sc_w;
        case 0b00001: /* amoswap.w */
            return inst_amoswap_w;
        case 0b00000: /* amoadd.w */
            return inst_amoadd_w;
        case 0b00100: /* amoxor.w */
            return inst_amoxor_w;
        case 0b01100: /* amoand.w */
            return inst_amoand_w;
        case 0b01000: /* amoor.w */
            return inst_amoor_w;
        case 0b10000: /* amomin.w */
            return inst_amomin_w;
        case 0b10100: /* amomax.w */
            return inst_amomax_w;
        case 0b11000: /* amominu.w */
            return inst_amominu_w;
        case 0b11100: /* amomaxu.w */
            return inst_amomaxu_w;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

#endif