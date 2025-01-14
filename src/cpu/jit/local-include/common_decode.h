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

#ifndef __JIT_COMMON_DECODE_H__
#define __JIT_COMMON_DECODE_H__

#include <cpu/inst.h>

static inline riscv_inst_t decode_invalid(uint32_t inst, riscv_ir_t *ir) {
    Panic("Invalid rule");
    return inst_err;
}

#endif
