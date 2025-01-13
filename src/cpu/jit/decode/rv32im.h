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

#ifndef __DECODE_RV32IM_H__
#define __DECODE_RV32IM_H__

#include <cpu/inst.h>

/* I-type */
static inline riscv_inst_t op_load(uint32_t inst, riscv_ir_t *ir) {
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->imm = decode_immI(inst);
    switch (decode_funct3(inst)) {
        case 0: /* lb */
            return inst_lb;
        case 1: /* lh */
            return inst_lh;
        case 2: /* lw */
            return inst_lw;
        case 4: /* lbu */
            return inst_lbu;
        case 5: /* lhu */
            return inst_lhu;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* I-type */
static inline riscv_inst_t op_imm(uint32_t inst, riscv_ir_t *ir) {
    const uint32_t __rd = decode_rd(inst);
    const uint32_t __imm = decode_immI(inst);
    ir->rd = __rd;
    ir->imm = __imm;
    ir->rs1 = decode_rs1(inst);

    if (unlikely(!__rd)) {
        /* nop */
        return inst_nop;
    }

    switch (decode_funct3(inst)) {
        case 0: /* addi */
            return inst_addi;
        case 1: /* slli */
            return inst_slli;
        case 2: /* slti */
            return inst_slti;
        case 3: /* sltiu */
            return inst_sltiu;
        case 4: /* xori */
            return inst_xori;
        case 5:
            if (__imm & ~0x1f) { /* srai */
                return inst_srai;
            } else { /* srli */
                return inst_srli;
            }
        case 6: /* ori */
            return inst_ori;
        case 7: /* andi */
            return inst_andi;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* U-type */
static inline riscv_inst_t op_auipc(uint32_t inst, riscv_ir_t *ir) {
    /* auipc */
    ir->rd = decode_rd(inst);
    ir->imm = decode_immU(inst);
    return inst_auipc;
}

/* S-type */
static inline riscv_inst_t op_store(uint32_t inst, riscv_ir_t *ir) {
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->imm = decode_immS(inst);
    switch (decode_funct3(inst)) {
        case 0: /* sb */
            return inst_sb;
        case 1: /* sh */
            return inst_sh;
        case 2: /* sw */
            return inst_sw;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* R-type */
static inline riscv_inst_t op_op(uint32_t inst, riscv_ir_t *ir) {
    const uint32_t funct3 = decode_funct3(inst);
    ir->rd = decode_rd(inst);
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    switch (decode_funct7(inst)) {
        case 0b0000000:
            switch (funct3) {
                case 0b000: /* add */
                    return inst_add;
                case 0b001: /* sll */
                    return inst_sll;
                case 0b010: /* slt */
                    return inst_slt;
                case 0b011: /* sltu */
                    return inst_sltu;
                case 0b100: /* xor */
                    return inst_xor;
                case 0b101: /* srl */
                    return inst_srl;
                case 0b110: /* or */
                    return inst_or;
                case 0b111: /* and */
                    return inst_and;
                default:
                    return inst_err;
            }
        case 0b0000001: /* RV32M */
            switch (funct3) {
                case 0b000: /* mul */
                    return inst_mul;
                case 0b001: /* mulh */
                    return inst_mulh;
                case 0b010: /* mulhsu */
                    return inst_mulhsu;
                case 0b011: /* mulhu */
                    return inst_mulhu;
                case 0b100: /* div */
                    return inst_div;
                case 0b101: /* divu */
                    return inst_divu;
                case 0b110: /* rem */
                    return inst_rem;
                case 0b111: /* remu */
                    return inst_remu;
                default:
                    return inst_err;
            }
        case 0b0100000:
            switch (funct3) {
                case 0b000: /* sub */
                    return inst_sub;
                case 0b101: /* sra */
                    return inst_sra;
                default:
                    return inst_err;
            }
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* U-type */
static inline riscv_inst_t op_lui(uint32_t inst, riscv_ir_t *ir) {
    /* lui */
    ir->rd = decode_rd(inst);
    ir->imm = decode_immU(inst);
    return inst_lui;
}

/* B-type */
static inline riscv_inst_t op_branch(uint32_t inst, riscv_ir_t *ir) {
    ir->rs1 = decode_rs1(inst);
    ir->rs2 = decode_rs2(inst);
    ir->imm = decode_immB(inst);
    switch (decode_funct3(inst)) {
        case 0: /* beq */
            return inst_beq;
        case 1: /* bne */
            return inst_bne;
        case 4: /* blt */
            return inst_blt;
        case 5: /* bge */
            return inst_bge;
        case 6: /* bltu */
            return inst_bltu;
        case 7: /* bgeu */
            return inst_bgeu;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

/* I-type */
static inline riscv_inst_t op_jalr(uint32_t inst, riscv_ir_t *ir) {
    /* jalr */
    ir->rs1 = decode_rs1(inst);
    ir->imm = decode_immI(inst);
    return inst_jalr;
}

/* J-type */
static inline riscv_inst_t op_jal(uint32_t inst, riscv_ir_t *ir) {
    /* jal */
    ir->rd = decode_rd(inst);
    ir->imm = decode_immJ(inst);
    return inst_jal;
}

static inline riscv_inst_t op_system(uint32_t inst, riscv_ir_t *ir) {
    switch (decode_funct3(inst)) {
        case 0:
            switch (decode_immI(inst)) {
                case 0: /* ecall */
                    return inst_ecall;
                case 1: /* ebreak */
                    return inst_ebreak;
                case 0x302: /* mret */
                    return inst_mret;
                default:
                    return inst_err;
            }
        case 1: /* csrrw */
            ir->imm = decode_immI(inst);
            return inst_csrrw;
        case 2: /* csrrs */
            ir->imm = decode_immI(inst);
            return inst_csrrs;
        default:
            return inst_err;
    }
    __builtin_unreachable();
    return inst_err;
}

#endif
