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

#ifndef __RV32IM_H__
#define __RV32IM_H__

#include <cpu/fpu.h>
#include <utils/state.h>
#include <system.h>
#include "macros.h"

extern decode_t unimpl;

/* I-type */
static inline void op_load(uint32_t inst, exec_t *info) {
    switch (decode_funct3(inst)) {
        case 0: /* lb */
            R(rd) = (uint32_t)((int32_t)((int8_t)(Mr_b(R(rs1) + IMM(I)))));
            break;
        case 1: /* lh */
            R(rd) = (uint32_t)((int32_t)((int16_t)(Mr_s(R(rs1) + IMM(I)))));
            break;
        case 2: /* lw */
            R(rd) = Mr_w(R(rs1) + IMM(I));
            break;
        case 4: /* lbu */
            R(rd) = Mr_b(R(rs1) + IMM(I));
            break;
        case 5: /* lhu */
            R(rd) = Mr_s(R(rs1) + IMM(I));
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* I-type */
static inline void op_imm(uint32_t inst, exec_t *info) {
    const uint32_t __imm = IMM(I);
    const uint32_t __rd = decode_rd(inst);

    if (unlikely(!__rd)) {
        /* nop */
        return;
    }

    switch (decode_funct3(inst)) {
        case 0: /* addi */
            R(__rd) = R(rs1) + __imm;
            break;
        case 1: /* slli */
            R(__rd) = R(rs1) << (__imm & 0x3f);
            break;
        case 2: /* slti */
            R(__rd) = (sword_t)R(rs1) < (sword_t)__imm;
            break;
        case 3: /* sltiu */
            R(__rd) = R(rs1) < __imm;
            break;
        case 4: /* xori */
            R(__rd) = R(rs1) ^ __imm;
            break;
        case 5:
            if (__imm & ~0x1f) { /* srai */
                R(__rd) = (sword_t)R(rs1) >> (__imm & 0x3f);
            } else { /* srli */
                R(__rd) = R(rs1) >> (__imm & 0x3f);
            }
            break;
        case 6: /* ori */
            R(__rd) = R(rs1) | __imm;
            break;
        case 7: /* andi */
            R(__rd) = R(rs1) & __imm;
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* U-type */
static inline void op_auipc(uint32_t inst, exec_t *info) {
    /* auipc */
    R(rd) = PC + IMM(U);
}

/* S-type */
static inline void op_store(uint32_t inst, exec_t *info) {
    switch (decode_funct3(inst)) {
        case 0: /* sb */
            Mw_b(R(rs1) + IMM(S), R(rs2));
            break;
        case 1: /* sh */
            Mw_s(R(rs1) + IMM(S), R(rs2));
            break;
        case 2: /* sw */
            Mw_w(R(rs1) + IMM(S), R(rs2));
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* R-type */
static inline void op_op(uint32_t inst, exec_t *info) {
    const uint32_t funct3 = decode_funct3(inst);

    switch (decode_funct7(inst)) {
        case 0b0000000:
            switch (funct3) {
                case 0b000: /* add */
                    R(rd) = R(rs1) + R(rs2);
                    break;
                case 0b001: /* sll */
                    R(rd) = R(rs1) << R(rs2);
                    break;
                case 0b010: /* slt */
                    R(rd) = (sword_t)R(rs1) < (sword_t)R(rs2);
                    break;
                case 0b011: /* sltu */
                    R(rd) = R(rs1) < R(rs2);
                    break;
                case 0b100: /* xor */
                    R(rd) = R(rs1) ^ R(rs2);
                    break;
                case 0b101: /* srl */
                    R(rd) = R(rs1) >> (R(rs2) & 0x1f);
                    break;
                case 0b110: /* or */
                    R(rd) = R(rs1) | R(rs2);
                    break;
                case 0b111: /* and */
                    R(rd) = R(rs1) & R(rs2);
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        case 0b0000001: /* RV32M */
            switch (funct3) {
                case 0b000: /* mul */
                    R(rd) = R(rs1) * R(rs2);
                    break;
                case 0b001: /* mulh */
                    R(rd) = (int64_t)(sword_t)R(rs1) * (int64_t)(sword_t)R(rs2) >> 32;
                    break;
                case 0b010: /* mulhsu */
                    R(rd) = ((uint64_t)((int64_t)(int32_t)R(rs1) * (uint64_t)R(rs2))) >> 32;
                    break;
                case 0b011: /* mulhu */
                    R(rd) = (uint64_t)R(rs1) * (uint64_t)R(rs2) >> 32;
                    break;
                case 0b100: /* div */
                    R(rd) = (sword_t)R(rs1) / (sword_t)R(rs2);
                    break;
                case 0b101: /* divu */
                    R(rd) = R(rs1) / R(rs2);
                    break;
                case 0b110: /* rem */
                    R(rd) = (sword_t)R(rs1) % (sword_t)R(rs2);
                    break;
                case 0b111: /* remu */
                    R(rd) = R(rs1) % R(rs2);
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        case 0b0100000:
            switch (funct3) {
                case 0b000: /* sub */
                    R(rd) = R(rs1) - R(rs2);
                    break;
                case 0b101: /* sra */
                    R(rd) = (sword_t)R(rs1) >> (R(rs2) & 0x1f);
                    break;
            }
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* U-type */
static inline void op_lui(uint32_t inst, exec_t *info) {
    /* lui */
    R(rd) = IMM(U);
}

/* B-type */
static inline void op_branch(uint32_t inst, exec_t *info) {
    switch (decode_funct3(inst)) {
        case 0: /* beq */
            if (R(rs1) == R(rs2)) NPC = PC + IMM(B);
            break;
        case 1: /* bne */
            if (R(rs1) != R(rs2)) NPC = PC + IMM(B);
            break;
        case 4: /* blt */
            if ((sword_t)R(rs1) < (sword_t)R(rs2)) NPC = PC + IMM(B);
            break;
        case 5: /* bge */
            if ((sword_t)R(rs1) >= (sword_t)R(rs2)) NPC = PC + IMM(B);
            break;
        case 6: /* bltu */
            if (R(rs1) < R(rs2)) NPC = PC + IMM(B);
            break;
        case 7: /* bgeu */
            if (R(rs1) >= R(rs2)) NPC = PC + IMM(B);
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

/* I-type */
static inline void op_jalr(uint32_t inst, exec_t *info) {
    /* jalr */
    word_t t = PC + 4;
    NPC = (R(rs1) + IMM(I)) & ~1;
    R(rd) = t;
}

/* J-type */
static inline void op_jal(uint32_t inst, exec_t *info) {
    /* jal */
    R(rd) = PC + 4;
    NPC = PC + IMM(J);
}

static inline void op_system(uint32_t inst, exec_t *info) {
    word_t t, *c;
    switch (decode_funct3(inst)) {
        case 0:
            switch (IMM(I)) {
                case 0: /* ecall */
                    NPC = ISA_RAISE_INTR(0xb, PC);
                    break;
                case 1: /* ebreak */
                    SET_STATE(gpr(10) ? ABORT : END);
                    break;
                case 0x302: /* mret */
                    NPC = cpu.csr_mepc;
                    const uint32_t mstatus_mpie =
                        (cpu.csr_mstatus & MSTATUS_MPIE) >> MSTATUS_MPIE_SHIFT;
                    cpu.csr_mstatus |= (mstatus_mpie << MSTATUS_MIE_SHIFT);
                    cpu.csr_mstatus |= MSTATUS_MPIE;
                    break;
                default:
                    unimpl(inst, info);
                    break;
            }
            break;
        case 1: /* csrrw */
            c = csr_get_ptr(IMM(I));
            t = *c;
            *c = R(rs1);
            R(rd) = t;
            break;
        case 2: /* csrrs */
            c = csr_get_ptr(IMM(I));
            t = *c;
            *c = t | R(rs1);
            R(rd) = t;
            break;
        default:
            unimpl(inst, info);
            break;
    }
}

#endif
