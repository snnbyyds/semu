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

#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <string.h>

static const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

static const char *fprs[] = {
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
    "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
    "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"
};

void reg_val_display() {
    for (size_t i = 0; i < LENGTH(regs); i++) {
        printf("%-4s\t0x%08x\t%" PRIword "\n", regs[i], gpr(i), gpr(i));
    }
}

char *reg_val_to_name(size_t idx) {
    static char buffer[4];
    assert(idx < LENGTH(regs));
    strncpy(buffer, regs[idx], 4);
    return buffer;
}

void fpr_val_display() {
    for (size_t i = 0; i < LENGTH(fprs); i++) {
        printf("%-4s\t0x%08x\t%" PRIword "\n", fprs[i], fpr(i).v, fpr(i).v);
    }
}

char *fpr_val_to_name(size_t idx) {
    static char buffer[4];
    assert(idx < LENGTH(fprs));
    strncpy(buffer, fprs[idx], 4);
    return buffer;
}
