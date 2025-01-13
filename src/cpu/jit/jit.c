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

#include <cpu/jit.h>
#include <device/device.h>
#include <device/timer.h>
#include <memory.h>
#include <utils/state.h>

static cache_t *jit_cache = NULL;
static block_exec_t fallback_interpreter = NULL;

/* API Wrapper */
semu_api_t shared_api = {
#define macro(f) .f = f,
    SEMU_APIS(macro)
#undef macro
};

/* Set fallback interpreter */
void jit_set_fallback_interpreter(block_exec_t interpreter) {
    assert(interpreter);
    fallback_interpreter = interpreter;
}

/* Exec in JIT mode */
void jit_exec() {
    assert(jit_cache && fallback_interpreter);
    if (likely(semu_state.state == STOP)) {
        SET_STATE(RUNNING);
        resume_timers();
    } else {
        Panic("Bad state!");
    }
    exec_t info = { .pc = cpu.pc, .snpc = cpu.pc };
    while (1) {
        bool hot;
        uint8_t *code = jitcache_find(jit_cache, cpu.pc);

check_hot:
        hot = true;
        if (!code) {
            hot = jitcache_hot(jit_cache, cpu.pc);
            if (hot) {
                code = jit_compiler(jit_gen_c_code(cpu.pc), jit_cache);
            }
        }
        if (!hot) {
            code = (uint8_t *)fallback_interpreter;
        }

        while (1) {
            semu_state.branch_prop = none_branch;
            ((block_exec_t)code)(&semu_state, &info);

            // Now a block has finished exec (met a branch or process finished)
            if (semu_state.state != RUNNING) {
                goto end;
            }
            assert(semu_state.branch_prop != none_branch);
            code = jitcache_find(jit_cache, info.dnpc);
            if (!code) {
                break;
            }
        }
        cpu.pc = info.dnpc;
        goto check_hot;
    }

end:
    switch (semu_state.state) {
        case END:
            Info("Hit Good Trap at PC 0x%08" PRIaddr "", semu_state.halt_pc);
            stop_timers();
            break;
        case ABORT:
            Error("Hit Bad Trap at PC 0x%08" PRIaddr "", cpu.pc);
            stop_timers();
            break;
        case QUIT:
            stop_timers();
            break;
        default:
            break;
    }
}

void jit_init() {
    semu_state.branch_prop = none_branch;
    jit_cache = jitcache_new();
    Info("You are using jit mode!");
}
