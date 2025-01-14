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

extern uint64_t running_seconds;

static pthread_t jit_thread;
static pthread_attr_t attr;
static struct sched_param param;
static bool jit_ready = false;
static cache_t *jit_cache = NULL;

#define ADD_TO_ENV(name) \
    name,

/* Wrapper of SEMU APIs, like vaddr_read_w */
const jit_code_env_t env_wrapper = {
    JIT_CODE_ENV(ADD_TO_ENV)
};

#undef ADD_TO_ENV

static void *jit_child_thread(void *arg) {
    exec_t info = { .pc = CONFIG_RESET_VECTOR, .snpc = CONFIG_RESET_VECTOR };
    branch_prop_t branch_prop = non_branch;
    while (1) {
        uint8_t *code = NULL;
        if (jit_cache->status == cache_free) {
            code = jit_cache_find(jit_cache, cpu.pc);
        }
        if (!code) {
            if (jit_cache->status == cache_free && jit_cache_hot(jit_cache, cpu.pc)) {
                jit_translate(cpu.pc);
            }
            code = (uint8_t *)block_interpreter_run;
        }

        while (1) {
            branch_prop = ((block_exec_t)code)(&cpu, &semu_state, &info, &env_wrapper);
            if (likely(branch_prop != non_branch)) {
                code = NULL;
                if (jit_cache->status == cache_free) {
                    code = jit_cache_find(jit_cache, cpu.pc);
                }
                if (!code) {
                    code = (uint8_t *)block_interpreter_run;
                }
                branch_prop = non_branch;
                break;
            }

            assert(semu_state.state != RUNNING);
            return NULL;
        }
    }
    return NULL;
}

static void start_jit_thread() {
    semu_state.state = RUNNING;
    if (unlikely(!jit_ready)) {
        Panic("JIT Not ready!");
    }
    int res = pthread_create(&jit_thread, &attr,
        jit_child_thread, NULL);
    if (unlikely(res)) {
        Panic("Failed to create interpreter thread!");
    }
}

static void wait_jit_thread() {
    while (pthread_tryjoin_np(jit_thread, NULL) == EBUSY) {
        update_device();
    }
}

static void jit_exit() {
    jit_translater_stop();
    pthread_attr_destroy(&attr);
    stop_timers();
    Info("Simulation time: %" PRIu64 "\n", running_seconds);   
}

cache_t *get_jit_cache() {
    return jit_cache;
}

void jit_run() {
    start_jit_thread();
    wait_jit_thread();

    switch (semu_state.state) {
        case RUNNING: case STOP:
            Panic("Unexpected State!");
            break;
        case END:
            Info("Hit Good Trap at PC 0x%08" PRIaddr "", semu_state.halt_pc);
            jit_exit();
            break;
        case ABORT:
            Error("Hit Bad Trap at PC 0x%08" PRIaddr "", cpu.pc);
            jit_exit();
            break;
        case QUIT:
            jit_exit();
            break;
    }
}

void jit_init() {
    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    jit_cache = jit_cache_new();
    jit_ready = true;
    jit_init_translater();
    Info("JIT runner initialized!");
}
