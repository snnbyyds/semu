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

#include <jit.h>
#include <cpu/fpu.h>
#include <memory.h>
#include <string.h>
#include <utils/set.h>
#include <utils/stack.h>

/* RV32IM */
#include "local-include/rv32im_rule.h"

/* RV32FD */
#include "local-include/rv32fd_rule.h"

/* RV32A */
#include "local-include/rv32a_rule.h"

static inline void reset_code_block(code_block_t *block) {
    if (block->c_code) {
        free(block->c_code);
    }
    block->len = 0;
}

static inline void append_code_block(code_block_t *block, const char *str) {
    size_t strsize = strlen(str) + 1;
    if (unlikely(!block->c_code)) {
        block->c_code = (char *)malloc(strsize);
        block->len = strsize - 1;
        strcpy(block->c_code, str);
        return;
    }
    assert(block->len);
    block->c_code = (char *)realloc(block->c_code, block->len + strsize + 4);
    assert(!block->c_code[block->len]);
    strcpy(block->c_code + block->len, str);
    block->len += (strsize - 1);
}

#define CODEGEN_START \
    "#include <stdint.h>                                                          \n" \
    "#include <stdbool.h>                                                         \n" \
    "#include <inttypes.h>                                                        \n" \
    "#define NR_GPR 32                                                            \n" \
    "#define NR_FPR 32                                                            \n" \
    "#define MSTATUS_MIE_SHIFT 3                                                  \n" \
    "#define MSTATUS_MPIE_SHIFT 7                                                 \n" \
    "#define MSTATUS_MIE (1 << MSTATUS_MIE_SHIFT)                                 \n" \
    "#define MSTATUS_MPIE (1 << MSTATUS_MPIE_SHIFT)                               \n" \
    "typedef enum { direct_branch, indirect_branch, non_branch, invalid } branch_prop_t; \n" \
    "typedef struct { uint32_t v; } float32_t;                                    \n" \
    "typedef struct { uint64_t v; } float64_t;                                    \n" \
    "typedef union {                                                              \n" \
    "    float64_t _64;                                                           \n" \
    "    uint64_t v64;                                                            \n" \
    "    float32_t _32;                                                           \n" \
    "    uint32_t v;                                                              \n" \
    "} fpr_t;                                                                     \n" \
    "typedef uint32_t word_t;                                                     \n" \
    "typedef int32_t sword_t;                                                     \n" \
    "typedef word_t paddr_t;                                                      \n" \
    "typedef paddr_t vaddr_t;                                                     \n" \
    "typedef word_t ioaddr_t;                                                     \n" \
    "typedef enum {                                                               \n" \
    "    FFLAGS   = 0x001,                                                        \n" \
    "    FRM      = 0x002,                                                        \n" \
    "    FCSR     = 0x003,                                                        \n" \
    "    SATP     = 0x180,                                                        \n" \
    "    MSTATUS  = 0x300,                                                        \n" \
    "    MTVEC    = 0x305,                                                        \n" \
    "    MSCRATCH = 0x340,                                                        \n" \
    "    MEPC     = 0x341,                                                        \n" \
    "    MCAUSE   = 0x342                                                         \n" \
    "} CSR_Number;                                                                \n" \
    "typedef enum { RUNNING, STOP, END, QUIT, ABORT } state_t;                    \n" \
    "typedef struct {                                                             \n" \
    "    state_t state;                                                           \n" \
    "    vaddr_t halt_pc;                                                         \n" \
    "    word_t halt_code;                                                        \n" \
    "} EMU_State;                                                                 \n" \
    "typedef struct {                                                             \n" \
    "    vaddr_t pc;                                                              \n" \
    "    vaddr_t snpc;                                                            \n" \
    "    vaddr_t dnpc;                                                            \n" \
    "} exec_t;                                                                    \n" \
    "typedef struct {                                                             \n" \
    "    word_t gpr[NR_GPR];                                                      \n" \
    "    vaddr_t pc;                                                              \n" \
    "    fpr_t fpr[NR_FPR];                                                       \n" \
    "    uint32_t csr_fcsr;                                                       \n" \
    "    uint32_t csr_satp;                                                       \n" \
    "    uint32_t csr_mstatus;                                                    \n" \
    "    uint32_t csr_mtvec;                                                      \n" \
    "    uint32_t csr_mscratch;                                                   \n" \
    "    uint32_t csr_mepc;                                                       \n" \
    "    uint32_t csr_mcause;                                                     \n" \
    "    bool intr;                                                               \n" \
    "} CPU_State;                                                                 \n" \
    "typedef struct {                                                             \n" \
    "    float32_t (*f32_add)(float32_t, float32_t);                              \n" \
    "    float32_t (*f32_sub)(float32_t, float32_t);                              \n" \
    "    float32_t (*f32_mul)(float32_t, float32_t);                              \n" \
    "    float32_t (*f32_mulAdd)(float32_t, float32_t, float32_t);                \n" \
    "    float32_t (*f32_div)(float32_t, float32_t);                              \n" \
    "    float32_t (*f32_rem)(float32_t, float32_t);                              \n" \
    "    float32_t (*f32_sqrt)(float32_t);                                        \n" \
    "    bool (*f32_eq)(float32_t, float32_t);                                    \n" \
    "    bool (*f32_le)(float32_t, float32_t);                                    \n" \
    "    bool (*f32_lt)(float32_t, float32_t);                                    \n" \
    "    bool (*f32_le_quiet)(float32_t, float32_t);                              \n" \
    "    bool (*f32_lt_quiet)(float32_t, float32_t);                              \n" \
    "    bool (*f32_isSignalingNaN)(float32_t);                                   \n" \
    "    uint_fast32_t (*f32_to_ui32)(float32_t, uint_fast8_t, bool);             \n" \
    "    int_fast32_t (*f32_to_i32)(float32_t, uint_fast8_t, bool);               \n" \
    "    float64_t (*f32_to_f64)(float32_t);                                      \n" \
    "    uint_fast32_t (*f64_to_ui32)(float64_t, uint_fast8_t, bool);             \n" \
    "    int_fast32_t (*f64_to_i32)(float64_t, uint_fast8_t, bool);               \n" \
    "    float32_t (*f64_to_f32)(float64_t);                                      \n" \
    "    float64_t (*f64_add)(float64_t, float64_t);                              \n" \
    "    float64_t (*f64_sub)(float64_t, float64_t);                              \n" \
    "    float64_t (*f64_mul)(float64_t, float64_t);                              \n" \
    "    float64_t (*f64_mulAdd)(float64_t, float64_t, float64_t);                \n" \
    "    float64_t (*f64_div)(float64_t, float64_t);                              \n" \
    "    float64_t (*f64_rem)(float64_t, float64_t);                              \n" \
    "    float64_t (*f64_sqrt)(float64_t);                                        \n" \
    "    bool (*f64_eq)(float64_t, float64_t);                                    \n" \
    "    bool (*f64_le)(float64_t, float64_t);                                    \n" \
    "    bool (*f64_lt)(float64_t, float64_t);                                    \n" \
    "    bool (*f64_le_quiet)(float64_t, float64_t);                              \n" \
    "    bool (*f64_lt_quiet)(float64_t, float64_t);                              \n" \
    "    bool (*f64_isSignalingNaN)(float64_t);                                   \n" \
    "    float64_t (*ui32_to_f64)(uint32_t);                                      \n" \
    "    float32_t (*ui32_to_f32)(uint32_t);                                      \n" \
    "    float32_t (*i32_to_f32)(int32_t);                                        \n" \
    "    float64_t (*i32_to_f64)(int32_t);                                        \n" \
    "    uint32_t (*classify_s)(uint32_t);                                        \n" \
    "    bool (*is_nan)(uint32_t);                                                \n" \
    "    uint32_t (*classify_d)(uint64_t);                                        \n" \
    "    bool (*is_nan64)(uint64_t);                                              \n" \
    "    void (*set_rounding_mode)(uint32_t);                                     \n" \
    "    void (*set_fflag)();                                                     \n" \
    "    float32_t (*unboxF32)(fpr_t);                                            \n" \
    "    void (*set_fpr_nan_box_upper_bits)(int);                                 \n" \
    "    uint32_t *(*csr_get_ptr)(CSR_Number);                                    \n" \
    "    uint64_t (*vaddr_read_d)(vaddr_t);                                       \n" \
    "    uint32_t (*vaddr_read_w)(vaddr_t);                                       \n" \
    "    uint16_t (*vaddr_read_s)(vaddr_t);                                       \n" \
    "    uint8_t (*vaddr_read_b)(vaddr_t);                                        \n" \
    "    void (*vaddr_write_d)(vaddr_t, uint64_t);                                \n" \
    "    void (*vaddr_write_w)(vaddr_t, uint32_t);                                \n" \
    "    void (*vaddr_write_s)(vaddr_t, uint16_t);                                \n" \
    "    void (*vaddr_write_b)(vaddr_t, uint8_t);                                 \n" \
    "} jit_code_env_t;                                                            \n" \
    "#define ACCESS_F32(idx) (cpu->fpr[idx]._32)                                  \n" \
    "#define ACCESS_F64(idx) (cpu->fpr[idx]._64)                                  \n" \
    "#define R(i) cpu->gpr[i]                                                     \n" \
    "#define F(i) cpu->fpr[i]                                                     \n" \
    "#define F32(i) ACCESS_F32(i)                                                 \n" \
    "#define F64(i) ACCESS_F64(i)                                                 \n" \
    "#define Mr_d env->vaddr_read_d                                               \n" \
    "#define Mr_w env->vaddr_read_w                                               \n" \
    "#define Mr_s env->vaddr_read_s                                               \n" \
    "#define Mr_b env->vaddr_read_b                                               \n" \
    "#define Mw_d env->vaddr_write_d                                              \n" \
    "#define Mw_w env->vaddr_write_w                                              \n" \
    "#define Mw_s env->vaddr_write_s                                              \n" \
    "#define Mw_b env->vaddr_write_b                                              \n" \
    "#define ISA_RAISE_INTR(NO, EPC)                                            \\\n" \
    "    ({                                                                     \\\n" \
    "        const uint32_t mstatus_mie =                                       \\\n" \
                "(cpu->csr_mstatus & MSTATUS_MIE) >> MSTATUS_MIE_SHIFT;         \\\n" \
    "        cpu->csr_mstatus |= (mstatus_mie << MSTATUS_MPIE_SHIFT);           \\\n" \
    "        cpu->csr_mstatus &= ~MSTATUS_MIE;                                  \\\n" \
    "        cpu->csr_mepc = (EPC);                                             \\\n" \
    "        cpu->csr_mcause = (NO);                                            \\\n" \
    "        cpu->csr_mtvec;                                                    \\\n" \
    "    })                                                                       \n" \
    "#define ISA_QUERY_INTR() ((cpu->intr && ((cpu->csr_mstatus) & (1 << 3))) ? ((cpu->intr = 0), 0x80000007) : -1) \n" \
    "#define SET_STATE(S)                                                       \\\n" \
    "    ({                                                                     \\\n" \
    "        state->state = (S);                                                \\\n" \
        "state->halt_pc = cpu->pc;                                              \\\n" \
        "state->halt_code = cpu->gpr[10];                                       \\\n" \
    "    })                                                                       \n" \
    "#define HANDLE_INTR() \\\n" \
    "({ \\\n" \
    "intr = ISA_QUERY_INTR();\\\n" \
    "if (intr != -1) {\\\n" \
    "    cpu->pc = ISA_RAISE_INTR(intr, npc);\\\n" \
    "    return indirect_branch;\\\n" \
    "}\\\n" \
    "})\n\n" \
    "branch_prop_t block(CPU_State *cpu, EMU_State *restrict state,               \n" \
    "                    exec_t *restrict info, const jit_code_env_t *env) {      \n" \
    "    register vaddr_t npc __attribute__((unused)) = cpu->pc;                  \n" \
    "    word_t *c __attribute__((unused));                                       \n " \
    "    word_t intr = -1;                                                        \n"

#define CODEGEN_END \
    "    return invalid;                                                          \n" \
    "}                                                                            \n"

static stack32_t stack;

static vaddr_t jit_gen_code_for_inst(code_block_t *block, vaddr_t pc, riscv_inst_t inst_type, riscv_ir_t *ir) {
    static char buffer[262144];

#define macro(name) [inst_ ## name] = gen_ ## name,
    static void (*rules[])(stack32_t *, char *, vaddr_t, riscv_ir_t *) = {
        RV_INST(macro)
    };
#undef macro

    rules[inst_type](&stack, buffer, pc, ir);

    /* Append buffer to code block */
    append_code_block(block, buffer);
    return -1;
}

code_block_t *jit_gen_c_code(vaddr_t start_pc) {
    static code_block_t block = CODE_BLOCK_INITIALIZER;
    static set32_t set;
    static char buffer[32];

    reset_code_block(&block);
    stack32_init(&stack);
    set32_reset(&set);

    append_code_block(&block, CODEGEN_START);
    sprintf(buffer, "goto __%08x;\n", start_pc);
    append_code_block(&block, buffer);

    stack32_push(&stack, start_pc);
    vaddr_t pc = -1;
    while ((pc = stack32_pop(&stack)) != -1) {
        if (!set32_add(&set, pc)) {
            continue;
        }
        riscv_ir_t ir;
        riscv_inst_t inst_type = jit_decode_inst(vaddr_ifetch(pc), &ir);
        jit_gen_code_for_inst(&block, pc, inst_type, &ir);
    }
    
    append_code_block(&block, CODEGEN_END);
    // FILE *fp = fopen("/home/sn/workspace/cpl-final/temp.c", "r+");
    // fprintf(fp, "%s", block.c_code);
    return &block;
}
