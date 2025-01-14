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

#ifndef __JIT_RV32IM_RULE_H__
#define __JIT_RV32IM_RULE_H__

#include <cpu/inst.h>
#include <jit.h>
#include "common_rule.h"

/* nop */
static inline void gen_nop(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    APPLY_TEMPLATE(pc, pc + 4, ";");
    ADD_NEXT(pc + 4);
}

/* op_load */
static inline void gen_lb(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (uint32_t)((int32_t)((int8_t)(Mr_b(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 ")))));",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_lh(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (uint32_t)((int32_t)((int16_t)(Mr_s(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 ")))));",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_lw(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = Mr_w(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_lbu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = Mr_b(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_lhu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = Mr_s(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

/* op_imm */
static inline void gen_addi(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 ");",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_slli(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) << (UINT32_C(%" PRIu32 ") & 0x3f);",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_slti(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (sword_t)R(UINT32_C(%" PRIu32 ")) < (sword_t)UINT32_C(%" PRIu32 ");",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sltiu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) < UINT32_C(%" PRIu32 ");",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_xori(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
    "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) ^ UINT32_C(%" PRIu32 ");",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_srai(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (sword_t)R(UINT32_C(%" PRIu32 ")) >> (UINT32_C(%" PRIu32 ") & 0x3f);",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_srli(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) >> (UINT32_C(%" PRIu32 ") & 0x3f);",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_ori(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) | UINT32_C(%" PRIu32 ");",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_andi(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) & UINT32_C(%" PRIu32 ");",
        ir->rd, ir->rs1, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

/* op_auipc */
static inline void gen_auipc(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = UINT32_C(%" PRIu32 ") + UINT32_C(%" PRIu32 ");",
        ir->rd, pc, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

/* op_store */
static inline void gen_sb(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "Mw_b(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 "), R(UINT32_C(%" PRIu32 ")));",
        ir->rs1, ir->imm, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sh(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "Mw_s(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 "), R(UINT32_C(%" PRIu32 ")));",
        ir->rs1, ir->imm, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sw(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "Mw_w(R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 "), R(UINT32_C(%" PRIu32 ")));",
        ir->rs1, ir->imm, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

/* op_op */
static inline void gen_add(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) + R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sll(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) << R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_slt(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (sword_t)R(UINT32_C(%" PRIu32 ")) < (sword_t)R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sltu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) < R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_xor(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) ^ R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_srl(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) >> (R(UINT32_C(%" PRIu32 ")) & 0x1f);",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_or(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) | R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_and(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) & R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_mul(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) * R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_mulh(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (int64_t)(sword_t)R(UINT32_C(%" PRIu32 ")) * (int64_t)(sword_t)R(UINT32_C(%" PRIu32 ")) >> 32;",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_mulhsu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = ((uint64_t)((int64_t)(int32_t)R(UINT32_C(%" PRIu32 ")) * (uint64_t)R(UINT32_C(%" PRIu32 ")))) >> 32;",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_mulhu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (uint64_t)R(UINT32_C(%" PRIu32 ")) * (uint64_t)R(UINT32_C(%" PRIu32 ")) >> 32;",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_div(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (sword_t)R(UINT32_C(%" PRIu32 ")) / (sword_t)R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_divu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) / R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_rem(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (sword_t)R(UINT32_C(%" PRIu32 ")) %% (sword_t)R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_remu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) %% R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sub(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = R(UINT32_C(%" PRIu32 ")) - R(UINT32_C(%" PRIu32 "));",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_sra(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp,
        "R(UINT32_C(%" PRIu32 ")) = (sword_t)R(UINT32_C(%" PRIu32 ")) >> (R(UINT32_C(%" PRIu32 ")) & 0x1f);",
        ir->rd, ir->rs1, ir->rs2);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

/* op_lui */
static inline void gen_lui(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[128];
    sprintf(tmp, "R(UINT32_C(%" PRIu32 ")) = UINT32_C(%" PRIu32 ");", ir->rd, ir->imm);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

/* op_branch */
static inline void gen_beq(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    if (R(UINT32_C(%" PRIu32 ")) == R(UINT32_C(%" PRIu32 "))) {\n"
        "        npc = 0x%08x;\n"
        "        goto __%08x;\n"
        "    }\n"
        "    npc = 0x%08x;\n"
        "    goto __%08x;\n"
        "}\n",
        pc, ir->rs1, ir->rs2,
        pc + ir->imm, pc + ir->imm,
        pc + 4, pc + 4);
    ADD_NEXT(pc + 4);
    ADD_NEXT(pc + ir->imm);
}

static inline void gen_bne(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    if (R(UINT32_C(%" PRIu32 ")) != R(UINT32_C(%" PRIu32 "))) {\n"
        "        npc = 0x%08x;\n"
        "        goto __%08x;\n"
        "    }\n"
        "    npc = 0x%08x;\n"
        "    goto __%08x;\n"
        "}\n",
        pc, ir->rs1, ir->rs2,
        pc + ir->imm, pc + ir->imm,
        pc + 4, pc + 4);
    ADD_NEXT(pc + 4);
    ADD_NEXT(pc + ir->imm);
}

static inline void gen_blt(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    if ((sword_t)R(UINT32_C(%" PRIu32 ")) < (sword_t)R(UINT32_C(%" PRIu32 "))) {   \n"
        "        npc = 0x%08x;\n"
        "        goto __%08x;\n"
        "    }\n"
        "    npc = 0x%08x;\n"
        "    goto __%08x;\n"
        "}\n",
        pc, ir->rs1, ir->rs2,
        pc + ir->imm, pc + ir->imm,
        pc + 4, pc + 4);
    ADD_NEXT(pc + 4);
    ADD_NEXT(pc + ir->imm);
}

static inline void gen_bge(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    if ((sword_t)R(UINT32_C(%" PRIu32 ")) >= (sword_t)R(UINT32_C(%" PRIu32 "))) {\n"
        "        npc = 0x%08x;\n"
        "        goto __%08x;\n"
        "    }\n"
        "    npc = 0x%08x;\n"
        "    goto __%08x;\n"
        "}\n",
        pc, ir->rs1, ir->rs2,
        pc + ir->imm, pc + ir->imm,
        pc + 4, pc + 4);
    ADD_NEXT(pc + 4);
    ADD_NEXT(pc + ir->imm);
}

static inline void gen_bltu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    if (R(UINT32_C(%" PRIu32 ")) < R(UINT32_C(%" PRIu32 "))) {\n"
        "        npc = 0x%08x;\n"
        "        goto __%08x;\n"
        "    }\n"
        "    npc = 0x%08x;\n"
        "    goto __%08x;\n"
        "}\n",
        pc, ir->rs1, ir->rs2,
        pc + ir->imm, pc + ir->imm,
        pc + 4, pc + 4);
    ADD_NEXT(pc + 4);
    ADD_NEXT(pc + ir->imm);
}

static inline void gen_bgeu(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    if (R(UINT32_C(%" PRIu32 ")) >= R(UINT32_C(%" PRIu32 "))) {\n"
        "        npc = 0x%08x;\n"
        "        goto __%08x;\n"
        "    }\n"
        "    npc = 0x%08x;\n"
        "    goto __%08x;\n"
        "}\n",
        pc, ir->rs1, ir->rs2,
        pc + ir->imm, pc + ir->imm,
        pc + 4, pc + 4);
    ADD_NEXT(pc + 4);
    ADD_NEXT(pc + ir->imm);
}

/* op_jalr */
static inline void gen_jalr(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {\n"
        HANDLE_INTR
        "    word_t t = UINT32_C(%" PRIu32 ") + 4;\n"
        "    npc = (R(UINT32_C(%" PRIu32 ")) + UINT32_C(%" PRIu32 ")) & ~1;\n"
        "    R(UINT32_C(%" PRIu32 ")) = t;\n"
        "    cpu->pc = npc;\n"
        "    return indirect_branch;\n"
        "}\n",
        pc, pc, ir->rs1, ir->imm, ir->rd);
}

/* op_jal */
static inline void gen_jal(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {                              \n"
        HANDLE_INTR
        "    R(UINT32_C(%" PRIu32 ")) = UINT32_C(%" PRIu32 ") + 4; \n"
        "    npc = UINT32_C(%" PRIu32 ") + UINT32_C(%" PRIu32 ");  \n"
        "    goto __%08x;                      \n"
        "}                                     \n",
        pc, ir->rd, pc, pc, ir->imm, pc + ir->imm);
    ADD_NEXT(pc + ir->imm);
}

/* op_system */
static inline void gen_ecall(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {                               \n"
        "    cpu->pc = ISA_RAISE_INTR(0xb, npc); \n"
        "    return indirect_branch;             \n"
        "}                                       \n",
        pc);
}

static inline void gen_ebreak(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {                             \n"
        "    SET_STATE(cpu->gpr[10] ? ABORT : END); \n"
        "    return non_branch;                \n"
        "}                                     \n",
        pc);
}

static inline void gen_mret(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    sprintf(buffer,
        "__%08x: {                                                        \n"
        "    cpu->pc = cpu->csr_mepc;                                     \n"
        "    const uint32_t mstatus_mpie =                                \n"
        "        (cpu->csr_mstatus & MSTATUS_MPIE) >> MSTATUS_MPIE_SHIFT; \n"
        "    cpu->csr_mstatus |= (mstatus_mpie << MSTATUS_MIE_SHIFT);     \n"
        "    cpu->csr_mstatus |= MSTATUS_MPIE;                            \n"
        "    return indirect_branch;                                      \n"
        "}                                                                \n",
        pc);
}

static inline void gen_csrrw(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[1024];
    sprintf(tmp,
        "__%08x: {                         \n"
        HANDLE_INTR
        "    c = csr_get_ptr(UINT32_C(%" PRIu32 ")); \n"
        "    word_t t = *c;                \n"
        "    *c = R(UINT32_C(%" PRIu32 "))           \n"
        "    R(UINT32_C(%" PRIu32 ")) = t            \n"
        "}                                 \n",
        pc, ir->imm, ir->rs1, ir->rd);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

static inline void gen_csrrs(stack32_t *stack, char *buffer, vaddr_t pc, riscv_ir_t *ir) {
    char tmp[1024];
    sprintf(tmp,
        "__%08x: {                         \n"
        HANDLE_INTR
        "    c = csr_get_ptr(UINT32_C(%" PRIu32 ")); \n"
        "    word_t t = *c;                \n"
        "    *c = R(UINT32_C(%" PRIu32 ")) | t       \n"
        "    R(UINT32_C(%" PRIu32 ")) = t            \n"
        "}                                 \n",
        pc, ir->imm, ir->rs1, ir->rd);
    APPLY_TEMPLATE(pc, pc + 4, tmp);
    ADD_NEXT(pc + 4);
}

#endif
