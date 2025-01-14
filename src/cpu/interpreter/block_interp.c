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

#include <cpu/inst.h>
#include <interpreter.h>
#include <utils/state.h>
#include <system.h>

static bool interpreter_ready = false;

void inst_exec_once(exec_t *restrict, branch_prop_t *prop);

branch_prop_t block_interpreter_run(CPU_State *dummy_cpu, EMU_State *restrict dummy_state, exec_t *restrict info, const jit_code_env_t *dummy_env) {
    if (unlikely(!interpreter_ready)) {
        Panic("Block interpreter not ready!");
    }
    branch_prop_t prop = non_branch;
    exec_t exec_info;
    while (prop == non_branch) {
        exec_info.pc = exec_info.snpc = cpu.pc;
        inst_exec_once(&exec_info, &prop);
        cpu.pc = exec_info.dnpc;

        word_t intr = ISA_QUERY_INTR();
        if (intr != -1) {
            cpu.pc = ISA_RAISE_INTR(intr, cpu.pc);
            return indirect_branch;
        }

        if (unlikely(semu_state.state != RUNNING)) {
            return non_branch;
        }
    }
    return prop;
}

void init_block_interpreter() {
    interpreter_ready = true;
}
