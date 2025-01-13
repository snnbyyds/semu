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
#include <cpu/jit.h>
#include <device/device.h>
#include <device/timer.h>
#include <errno.h>
#include <string.h>
#include <system.h>
#include <utils/difftest.h>
#include <utils/state.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <pthread.h>

static pthread_t thread_cpu_exec;
static pthread_attr_t attr;
static struct sched_param param;
static uint64_t step_count = 0;

void inst_exec_once(exec_t *restrict info, uint32_t inst, branch_prop_t *prop);

__attribute__((always_inline))
static inline void interpreter_step_exec_once() {
    // exec the inst
    exec_t exec_info = { .snpc = cpu.pc, .pc = cpu.pc };
    inst_exec_once(&exec_info, vaddr_ifetch(exec_info.snpc), NULL);
    cpu.pc = exec_info.dnpc;
#ifdef CONFIG_DIFFTEST
    difftest_exec_once();
#endif
}

/* Exec child thread */
static void *interpreter_step_exec_thread(void *arg) {
    uint64_t step = (uint64_t)arg;
    register uint64_t i = 0;
    for (; i < step; i++) {
        interpreter_step_exec_once();
        word_t intr = ISA_QUERY_INTR();
        if (intr != -1) {
            cpu.pc = ISA_RAISE_INTR(intr, cpu.pc);
        }

#ifdef CONFIG_PAUSE_PC
        if (unlikely(sdb_pause_pc != -1 && cpu.pc == sdb_pause_pc)) {
            Warn("Hit pc 0x%08" PRIu64 "", sdb_pause_pc);
            SET_STATE(STOP);
        }
#endif

        if (unlikely(semu_state.state != RUNNING)) {
            i++;
            break;
        }
    }
    step_count += i;
    return NULL;
}

static void halt_step_interpreter() {
    stop_timers();
    pthread_attr_destroy(&attr);
    Info("%" PRIu64 " inst simulated", step_count);
    if (running_seconds > 4) {
        Info("Simulation Speed: %" PRIu64 " inst/s",
            step_count / running_seconds);
    }
}

static void start_step_interpreter_thread(uint64_t nr_exec) {
    int res = pthread_create(&thread_cpu_exec, &attr,
        interpreter_step_exec_thread, (void *)nr_exec);
    if (unlikely(res)) {
        Error("Failed to create cpu_exec thread!");
        SET_STATE(ABORT);
    }
}

static void wait_step_interpreter_thread() {
    while (pthread_tryjoin_np(thread_cpu_exec, NULL) == EBUSY) {
        update_device();
    }
}

/* Step-level interpreter
 * @step: Number of the steps
 */
void interpreter_exec(uint64_t step) {
    switch (semu_state.state) {
        case END: case ABORT: case QUIT:
            stop_timers();
            printf("Program has ended. Please restart semu.\n");
            return;
        default:
            SET_STATE(RUNNING);
            resume_timers();
            break;
    }

    start_step_interpreter_thread(step);
    wait_step_interpreter_thread();

    switch (semu_state.state) {
        case RUNNING:
            SET_STATE(STOP);
            stop_timers();
            break;
        case END:
            Info("Hit Good Trap at PC 0x%08" PRIaddr "", semu_state.halt_pc);
            halt_step_interpreter();
            break;
        case ABORT:
            Error("Hit Bad Trap at PC 0x%08" PRIaddr "", cpu.pc);
            halt_step_interpreter();
            break;
        case QUIT:
            halt_step_interpreter();
            break;
        default:
            break;
    }
}

/* Block level interpreter, works only in JIT/Block mode */
static void block_interpreter(EMU_State *restrict state, exec_t *restrict info) {
    branch_prop_t prop = none_branch;
    while (prop == none_branch) {
        
    }
}

/* Initialize the interpreter */
void init_interpreter(interpreter_mode_t interpreter_mode) {
    if (interpreter_mode == block_mode) {
        jit_set_fallback_interpreter(block_interpreter);
        Info("Registered JIT fallback interpreter %p", block_interpreter);
        return;
    }

    assert(interpreter_mode == step_mode);
    Info("You are using interpreter(step) mode!");

    /* Set up thread */
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
}
