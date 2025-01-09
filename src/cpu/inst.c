#include <memory.h>
#include <utils/state.h>

#include "insts/rv32im.h" /* RV32I and RV32M */
#include "insts/rv32f.h"  /* RV32F */

static uint32_t inst;

void itrace(exec_t *info, uint32_t inst);

static void op_err(uint32_t inst, exec_t *info) {
    Error("Failed to exec inst 0x%" PRIx32 " at PC 0x%" PRIaddr "", inst, cpu.pc);
    SET_STATE(ABORT);
}

decode_t unimpl = op_err;

__attribute__((always_inline))
static inline void inst_decode_and_exec(exec_t *info) {
    /* Basic RV32IMF opcode map */
    #define ______ op_err
    static const decode_t opcode_map[] = {
        [0] = op_load, [1] = op_load_fp, [2] = ______, [3] = ______,
        [4] = op_imm, [5] = op_auipc, [6] = ______, [7] = ______,
        [8] = op_store, [9] = op_store_fp, [10] = ______, [11] = ______,
        [12] = op_op, [13] = op_lui, [14] = ______, [15] = ______,
        [16] = op_madd, [17] = op_msub, [18] = op_nmsub, [19] = op_nmadd,
        [20] = op_op_fp, [21] = ______, [22] = ______, [23] = ______,
        [24] = op_branch, [25] = op_jalr, [26] = ______, [27] = op_jal,
        [28] = op_system, [29] = ______, [30] = ______, [31] = ______,
    };
    #undef ______

    /* Decode the instruction */
    opcode_map[(inst & INST_6_2) >> 2](inst, info);

    /* Reset $zero */
    gpr(0) = 0;
}

void inst_exec_once(exec_t *info) {
    /* Fetch instruction from memory */
    inst = vaddr_ifetch(info->snpc);

    itrace(info, inst);
    info->snpc += 4;
    info->dnpc = info->snpc;

    /* Decode and exec */
    inst_decode_and_exec(info);
}
