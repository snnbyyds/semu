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

#include "local-include/common_decode.h"

/* RV32A */
#include "local-include/rv32a_decode.h"

/* RV32FD */
#include "local-include/rv32fd_decode.h"

/* RV32IM */
#include "local-include/rv32im_decode.h"

riscv_inst_t jit_decode_inst(uint32_t inst, riscv_ir_t *ir) {
    /* Basic RV32IMAFD decode map */
    #define ___________________ decode_invalid
    static riscv_inst_t (*map[])(uint32_t, riscv_ir_t *) = {
        [0]  = decode_op_load,   [1]  = decode_op_load_fp,   [2]  = ___________________, [3]  = ___________________,
        [4]  = decode_op_imm,    [5]  = decode_op_auipc,     [6]  = ___________________, [7]  = ___________________,
        [8]  = decode_op_store,  [9]  = decode_op_store_fp,  [10] = ___________________, [11] = decode_op_amo,
        [12] = decode_op_op,     [13] = decode_op_lui,       [14] = ___________________, [15] = ___________________,
        [16] = decode_op_madd,   [17] = decode_op_msub,      [18] = decode_op_nmsub,     [19] = decode_op_nmadd,
        [20] = decode_op_op_fp,  [21] = ___________________, [22] = ___________________, [23] = ___________________,
        [24] = decode_op_branch, [25] = decode_op_jalr,      [26] = ___________________, [27] = decode_op_jal,
        [28] = decode_op_system, [29] = ___________________, [30] = ___________________, [31] = ___________________,
    };
    #undef ___________________

    return map[(inst & INST_6_2) >> 2](inst, ir);
}
