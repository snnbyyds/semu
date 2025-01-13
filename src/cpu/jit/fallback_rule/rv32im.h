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

#ifndef __EXEC_RV32IM_H__
#define __EXEC_RV32IM_H__

#include <cpu/fpu.h>
#include <cpu/inst.h>
#include <system.h>
#include <utils/state.h>
#include "macros.h"

static inline void func_nop(exec_t *info, riscv_ir_t *ir) {
}

static inline void func_lb(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (uint32_t)((int32_t)((int8_t)(Mr_b(R(rs1) + imm))));
}

static inline void func_lh(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (uint32_t)((int32_t)((int16_t)(Mr_s(R(rs1) + imm))));
}

static inline void func_lw(exec_t *info, riscv_ir_t *ir) {
    R(rd) = Mr_w(R(rs1) + imm);
}

static inline void func_lbu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = Mr_b(R(rs1) + imm);
}

static inline void func_lhu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = Mr_s(R(rs1) + imm);
}

static inline void func_addi(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) + imm;
}

static inline void func_slli(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) << (imm & 0x3f);
}

static inline void func_slti(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (sword_t)R(rs1) < (sword_t)imm;
}

static inline void func_sltiu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) < imm;
}

static inline void func_xori(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) ^ imm;
}

static inline void func_srai(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (sword_t)R(rs1) >> (imm & 0x3f);
}

static inline void func_srli(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) >> (imm & 0x3f);
}

static inline void func_ori(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) | imm;
}

static inline void func_andi(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) & imm;
}

static inline void func_auipc(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) + imm;
}

static inline void func_sb(exec_t *info, riscv_ir_t *ir) {
    Mw_b(R(rs1) + imm, R(rs2));
}

static inline void func_sh(exec_t *info, riscv_ir_t *ir) {
    Mw_s(R(rs1) + imm, R(rs2));
}

static inline void func_sw(exec_t *info, riscv_ir_t *ir) {
    Mw_w(R(rs1) + imm, R(rs2));
}

static inline void func_add(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) + R(rs2);
}

static inline void func_sll(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) << R(rs2);
}

static inline void func_slt(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (sword_t)R(rs1) < (sword_t)R(rs2);
}

static inline void func_sltu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) < R(rs2);
}

static inline void func_xor(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) ^ R(rs2);
}

static inline void func_srl(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) >> (R(rs2) & 0x1f);
}

static inline void func_or(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) | R(rs2);
}

static inline void func_and(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) & R(rs2);
}

static inline void func_mul(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) * R(rs2);
}

static inline void func_mulh(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (int64_t)(sword_t)R(rs1) * (int64_t)(sword_t)R(rs2) >> 32;
}

static inline void func_mulhsu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = ((uint64_t)((int64_t)(int32_t)R(rs1) * (uint64_t)R(rs2))) >> 32;
}

static inline void func_mulhu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (uint64_t)R(rs1) * (uint64_t)R(rs2) >> 32;
}

static inline void func_div(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (sword_t)R(rs1) / (sword_t)R(rs2);
}

static inline void func_divu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) / R(rs2);
}

static inline void func_rem(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (sword_t)R(rs1) % (sword_t)R(rs2);
}

static inline void func_remu(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) % R(rs2);
}

static inline void func_sub(exec_t *info, riscv_ir_t *ir) {
    R(rd) = R(rs1) - R(rs2);
}

static inline void func_sra(exec_t *info, riscv_ir_t *ir) {
    R(rd) = (sword_t)R(rs1) >> (R(rs2) & 0x1f);
}

static inline void func_lui(exec_t *info, riscv_ir_t *ir) {
    R(rd) = imm;
}

static inline void func_beq(exec_t *info, riscv_ir_t *ir) {
    if (R(rs1) == R(rs2)) NPC = PC + imm;
}

static inline void func_bne(exec_t *info, riscv_ir_t *ir) {
    if (R(rs1) != R(rs2)) NPC = PC + imm;
}

static inline void func_blt(exec_t *info, riscv_ir_t *ir) {
    if ((sword_t)R(rs1) < (sword_t)R(rs2)) NPC = PC + imm;
}

static inline void func_bge(exec_t *info, riscv_ir_t *ir) {
    if ((sword_t)R(rs1) >= (sword_t)R(rs2)) NPC = PC + imm;
}

static inline void func_bltu(exec_t *info, riscv_ir_t *ir) {
    if (R(rs1) < R(rs2)) NPC = PC + imm;
}

static inline void func_bgeu(exec_t *info, riscv_ir_t *ir) {
    if (R(rs1) >= R(rs2)) NPC = PC + imm;
}

static inline void func_jalr(exec_t *info, riscv_ir_t *ir) {
    R(rd) = PC + 4;
    NPC = PC + imm;
}

static inline void func_ecall(exec_t *info, riscv_ir_t *ir) {
    NPC = ISA_RAISE_INTR(0xb, PC);
}

static inline void func_ebreak(exec_t *info, riscv_ir_t *ir) {
    SET_STATE(gpr(10) ? ABORT : END);
}

static inline void func_mret(exec_t *info, riscv_ir_t *ir) {
    NPC = cpu.csr_mepc;
    const uint32_t mstatus_mpie =
        (cpu.csr_mstatus & MSTATUS_MPIE) >> MSTATUS_MPIE_SHIFT;
    cpu.csr_mstatus |= (mstatus_mpie << MSTATUS_MIE_SHIFT);
    cpu.csr_mstatus |= MSTATUS_MPIE;
}

static inline void func_csrrw(exec_t *info, riscv_ir_t *ir) {
    word_t t, *c;
    c = csr_get_ptr(imm);
    t = *c;
    *c = R(rs1);
    R(rd) = t;
}

static inline void func_csrrs(exec_t *info, riscv_ir_t *ir) {
    word_t t, *c;
    c = csr_get_ptr(imm);
    t = *c;
    *c = t | R(rs1);
    R(rd) = t;
}

#endif
