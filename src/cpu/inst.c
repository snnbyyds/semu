#include <memory.h>
#include <utils/state.h>

#include "insts/rv32im.h" /* RV32I and RV32M */
#include "insts/rv32f.h"  /* RV32F */
#include "insts/rv32c.h"  /* RV32C */

static uint32_t inst;

void itrace(exec_t *info, uint32_t inst);

static void op_err(uint32_t inst, exec_t *info) {
    Error("Failed to exec inst 0x%" PRIx32 " at PC 0x%" PRIaddr "", inst, cpu.pc);
    SET_STATE(ABORT);
}

decode_t unimpl = op_err;

__attribute__((always_inline))
static inline void inst_decode_and_exec(exec_t *info, bool compressed) {
    #define ____________ op_err

    /* Basic RV32IMF map */
    static const decode_t opcode_map[] = {
        [0]  = op_load,    [1]  = op_load_fp,   [2]  = ____________,   [3]  = ____________,
        [4]  = op_imm,     [5]  = op_auipc,     [6]  = ____________,   [7]  = ____________,
        [8]  = op_store,   [9]  = op_store_fp,  [10] = ____________,   [11] = ____________,
        [12] = op_op,      [13] = op_lui,       [14] = ____________,   [15] = ____________,
        [16] = op_madd,    [17] = op_msub,      [18] = op_nmsub,       [19] = op_nmadd,
        [20] = op_op_fp,   [21] = ____________, [22] = ____________,   [23] = ____________,
        [24] = op_branch,  [25] = op_jalr,      [26] = ____________,   [27] = op_jal,
        [28] = op_system,  [29] = ____________, [30] = ____________,   [31] = ____________,
    };

    /* Basic RV32C map */
    static const decode_t opcode_map_compressed[] = {
        [0]  = op_caddi4spn, [1]  = op_caddi,     [2]  = op_cslli,      [3]  = ____________,
        [4]  = ____________, [5]  = op_cjal,      [6]  = ____________,  [7]  = ____________,
        [8]  = op_clw,       [9]  = op_cli,       [10] = op_clwsp,      [11] = ____________,
        [12] = op_cflw,      [13] = op_clui,      [14] = op_cflwsp,     [15] = ____________,
        [16] = ____________, [17] = op_cmisc_alu, [18] = op_ccr,        [19] = ____________,
        [20] = ____________, [21] = op_cj,        [22] = ____________,  [23] = ____________,
        [24] = op_csw,       [25] = op_cbeqz,     [26] = op_cswsp,      [27] = ____________,
        [28] = op_cfsw,      [29] = op_cbnez,     [30] = op_cfswsp,     [31] = ____________,
    };

    #undef ____________

    /* Decode the instruction */
    if (compressed) {
        inst &= 0x0000ffff;
        opcode_map_compressed[(inst & FC_FUNCT3) >> 11 | (inst & FC_OPCODE)](inst, info);
    } else {
        opcode_map[(inst & INST_6_2) >> 2](inst, info);
    }

    /* Reset $zero */
    gpr(0) = 0;
}

void inst_exec_once(exec_t *info) {
    /* Fetch instruction from memory */
    inst = vaddr_ifetch(info->snpc);

    /* Judge whether the fetched inst is compressed (RV32C feature) */
    const bool compressed = ((inst & FC_OPCODE) != 3);

    itrace(info, inst);
    info->snpc += compressed ? 2 : 4;
    info->dnpc = info->snpc;

    /* Decode and exec */
    inst_decode_and_exec(info, compressed);
}
