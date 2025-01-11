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

#include <dlfcn.h>
#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <memory.h>
#include <utils/difftest.h>
#include <utils/state.h>

/*
 * Difftest Framework for SEMU
 * 
 * This difftest framework is an enhanced version derived from NEMU's framework.
 * It serves as a powerful debugging and verification tool for the SEMU RISC-V emulator.
 * 
 * Functionality:
 * - The difftest framework performs **differential testing** by running both SEMU (the DUT - Device Under Test) 
 *   and a reference implementation (e.g., NEMU or Spike) in parallel.
 * - After executing each instruction in SEMU, the framework compares the states of SEMU and the reference,
 *   including general-purpose registers, program counters, and optionally memory states.
 * - If a mismatch is detected, it halts execution and provides detailed information about the discrepancy,
 *   helping identify bugs or inconsistencies in the emulator's implementation.
 * 
 * This framework is especially useful for verifying complex instruction sets such as RV32A, RV32F, and RV32D,
 * as well as extensions in SEMU. It plays a critical role in maintaining the reliability of the emulator.
 */


#ifdef CONFIG_DIFFTEST

static bool difftest_init_state = true;

static void (*ref_difftest_init)(int port);
static void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n, bool direction);
static void (*ref_difftest_regcpy)(void *dut, bool direction);
static void (*ref_difftest_exec)(uint64_t n);
static void (*ref_difftest_raise_intr)(uint64_t NO);

static riscv_context_t ref_ctx = {};
static bool is_skip_ref = false;

#define FAIL(cond, ...) ({ \
    if (!cond) { \
        difftest_init_state = false; \
        __VA_ARGS__; \
    } \
})

static inline bool check_registers() {
    bool ret = true;
    /* Check gprs */
    for (size_t i = 0; i < NR_GPR; i++) {
        if (ref_ctx.gpr[i] != gpr(i)) {
            ret = false;
            Error("Detected mismatch for %s! ref: 0x%08x | dut: 0x%08x",
                reg_val_to_name(i), ref_ctx.gpr[i], gpr(i));
        }
    }
    /* Check fprs */
#ifdef CONFIG_DIFFTEST_REF_SPIKE
    for (size_t i = 0; i < NR_FPR; i++) {
        if (ref_ctx.fpr[i].v64 != fpr(i).v64) {
            ret = false;
            Error("Detected mismatch for %s! ref: 0x%08lx | dut: 0x%08lx",
                fpr_val_to_name(i), ref_ctx.fpr[i].v64, fpr(i).v64);
        }
    }
#endif
    return ret;
}

void difftest_skip_ref() {
    is_skip_ref = true;
}

void difftest_exec_once() {
    FAIL(!difftest_init_state, return);
    if (is_skip_ref) {
        /* Sync DUT registers to REF */
        ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
        is_skip_ref = false;
        return;
    }
    ref_difftest_exec(1);
    ref_difftest_regcpy(&ref_ctx, DIFFTEST_TO_DUT);
    if (!check_registers()) {
        SET_STATE(ABORT);
    }
}

void init_difftest(const char *ref_so, int port, size_t imgsize) {
    FAIL(!ref_so || !*ref_so, return);

    void *handle = dlopen(ref_so, RTLD_LAZY);
    FAIL(!handle, return);

    ref_difftest_init = dlsym(handle, "ref_difftest_init");
    ref_difftest_memcpy = dlsym(handle, "difftest_memcpy");
    ref_difftest_regcpy = dlsym(handle, "difftest_regcpy");
    ref_difftest_exec = dlsym(handle, "difftest_exec");
    ref_difftest_raise_intr = dlsym(handle, "difftest_raise_intr");
    FAIL(!ref_difftest_init || !ref_difftest_memcpy ||
        !ref_difftest_init || !ref_difftest_raise_intr,
        return);

    ref_difftest_init(port);

    ref_difftest_memcpy(CONFIG_RESET_VECTOR, GUEST_TO_HOST(CONFIG_RESET_VECTOR),
        imgsize, DIFFTEST_TO_REF);
}

#else

void difftest_exec_once() {}
void difftest_skip_ref() {}
void init_difftest(const char *ref_so, int port, size_t imgsize) {}

#endif
