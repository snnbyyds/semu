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

#include <memory.h>
#include <utils/state.h>

#include "local-include/rv32im.h" /* RV32I and RV32M */
#include "local-include/rv32a.h"  /* RV32A */
#include "local-include/rv32fd.h" /* RV32FD */

static uint32_t inst;

void itrace(exec_t *restrict info, uint32_t inst);

static void op_err(uint32_t inst, exec_t *restrict info, branch_prop_t *restrict branchprop) {
    Error("Failed to exec inst 0x%" PRIx32 " at PC 0x%" PRIaddr "", inst, cpu.pc);
    SET_STATE(ABORT);
}

decode_t unimpl = op_err;

__attribute__((always_inline))
static inline void inst_decode_and_exec(exec_t *restrict info, branch_prop_t *prop) {
    /* Basic RV32IMAFD opcode map */
    #define ____________ op_err
    static const decode_t opcode_map[] = {
        [0]  = op_load,   [1]  = op_load_fp,   [2]  = ____________, [3]  = ____________,
        [4]  = op_imm,    [5]  = op_auipc,     [6]  = ____________, [7]  = ____________,
        [8]  = op_store,  [9]  = op_store_fp,  [10] = ____________, [11] = op_amo,
        [12] = op_op,     [13] = op_lui,       [14] = ____________, [15] = ____________,
        [16] = op_madd,   [17] = op_msub,      [18] = op_nmsub,     [19] = op_nmadd,
        [20] = op_op_fp,  [21] = ____________, [22] = ____________, [23] = ____________,
        [24] = op_branch, [25] = op_jalr,      [26] = ____________, [27] = op_jal,
        [28] = op_system, [29] = ____________, [30] = ____________, [31] = ____________,
    };
    #undef ____________

    /* Decode the instruction */
    opcode_map[(inst & INST_6_2) >> 2](inst, info, prop);

    /* Reset $zero */
    gpr(0) = 0;
}

void inst_exec_once(exec_t *restrict info, branch_prop_t *prop) {
    /* Fetch instruction from memory */
    inst = vaddr_ifetch(info->snpc);

    itrace(info, inst);
    info->snpc += 4;
    info->dnpc = info->snpc;

    /* Decode and exec */
    inst_decode_and_exec(info, prop);
}
