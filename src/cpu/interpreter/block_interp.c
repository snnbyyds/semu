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

static bool interpreter_ready = false;

void block_interpreter_run(EMU_State *restrict state, exec_t *restrict info) {
    if (unlikely(!interpreter_ready)) {
        Panic("Block interpreter not ready!");
    }
}

void init_block_interpreter() {
    // TODO: Implement
    interpreter_ready = true;
}
