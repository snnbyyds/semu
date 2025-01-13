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
#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <string.h>

#define __USE_GNU
#include <pthread.h>

CPU_State cpu = {};
uint64_t running_seconds = 0;

#ifdef CONFIG_PAUSE_PC
uint64_t sdb_pause_pc = -1;
#endif

static const uint32_t builtin_img[] = {
    0x00000297, // auipc t0,0
    0x00028823, // sb  zero,16(t0)
    0x0102c503, // lbu a0,16(t0)
    0x00100073, // ebreak
    0x00000000
};

size_t builtin_img_size = sizeof(builtin_img);

static emu_mode_t mode = mode_notset;

emu_mode_t cpu_get_mode() {
    return mode;
}

/* Let the cpu exec some steps
 * Note that in jit mode, the step param
 * will be ignored.
 */
void cpu_exec(uint64_t step) {
    switch (mode) {
        case jit_mode:
            jit_exec();
            break;
        case interpreter_mode:
            interpreter_exec(step);
            break;
        default:
            __builtin_unreachable();
    }
}

/* Initialize the cpu */
void init_cpu(bool img_builtin, emu_mode_t m) {
    assert(m != mode_notset);
    memset(&cpu, 0, sizeof(cpu));
    cpu.pc = CONFIG_RESET_VECTOR;
    cpu.csr_mstatus = 0x1800;
    mode = m;
    Log("CPU Initialized!");

    if (m == interpreter_mode) {
        init_interpreter(step_mode);
    } else {
        init_interpreter(block_mode);
        jit_init();
    }

    // Load builtin image
    if (img_builtin) {
        Log("Loading builtin image to 0x%" PRIx64 "...", (uint64_t)CONFIG_RESET_VECTOR);
        memcpy(GUEST_TO_HOST(CONFIG_RESET_VECTOR), builtin_img, sizeof(builtin_img));
    }
}
