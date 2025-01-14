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

#include <interpreter.h>
#include <memory.h>
#include <cpu/cpu.h>
#include <cpu/inst.h>
#include <device/device.h>
#include <device/timer.h>
#include <errno.h>
#include <string.h>
#include <system.h>
#include <utils/difftest.h>
#include <utils/state.h>

#define __USE_GNU
#include <pthread.h>

uint64_t running_seconds = 0;

#ifdef CONFIG_PAUSE_PC
uint64_t sdb_pause_pc = -1;
#endif

static pthread_t interpreter_thread;
static pthread_attr_t attr;
static struct sched_param param;
static uint64_t step_count = 0;
static bool interpreter_ready = false;

void inst_exec_once(exec_t *restrict, branch_prop_t *prop);

static void *interpreter_child_thread(void *arg) {
    uint64_t step = (uint64_t)arg;
    branch_prop_t prop = non_branch;
    register uint64_t i = 0;
    exec_t exec_info;
    for (; i < step; i++) {
        exec_info.pc = exec_info.snpc = cpu.pc;
        inst_exec_once(&exec_info, &prop);
        cpu.pc = exec_info.dnpc;
#ifdef CONFIG_DIFFTEST
        difftest_exec_once();
#endif
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

static void start_interpreter_thread(uint64_t nr_steps) {
    if (unlikely(!interpreter_ready)) {
        Panic("Step interpreter hasn't be initialized!");
    }
    int res = pthread_create(&interpreter_thread, &attr,
        interpreter_child_thread, (void *)nr_steps);
    if (unlikely(res)) {
        Panic("Failed to create interpreter thread!");
    }
}

static void wait_interpreter_thread() {
    while (pthread_tryjoin_np(interpreter_thread, NULL) == EBUSY) {
        update_device();
    }
}

static void interpreter_exit() {
    Info("%" PRIu64 " inst simulated", step_count);
    if (running_seconds > 4) {
        Info("Simulation Speed: %" PRIu64 " inst/s", step_count / running_seconds);
    }
    pthread_attr_destroy(&attr);
    stop_timers();
}

void step_interpreter_exec(uint64_t step) {
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

    start_interpreter_thread(step);
    wait_interpreter_thread();

    switch (semu_state.state) {
        case RUNNING:
            SET_STATE(STOP);
            stop_timers();
        case END:
            Info("Hit Good Trap at PC 0x%08" PRIaddr "", semu_state.halt_pc);
            interpreter_exit();
            break;
        case ABORT:
            Error("Hit Bad Trap at PC 0x%08" PRIaddr "", cpu.pc);
            interpreter_exit();
            break;
        case QUIT:
            interpreter_exit();
            break;
        default:
            break;
    }
}

void init_step_interpreter() {
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    interpreter_ready = true;
    Info("step interpreter initialized!");
}
