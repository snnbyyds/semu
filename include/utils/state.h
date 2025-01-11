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

#ifndef __STATE_H__
#define __STATE_H__

#include <cpu/cpu.h>
#include <cpu/reg.h>

typedef enum { RUNNING, STOP, END, QUIT, ABORT } state_t;

typedef struct {
    state_t state;
    vaddr_t halt_pc;
    word_t halt_code;
} EMU_State;

bool is_good_exit_state();

extern EMU_State semu_state;

#define SET_STATE(S) ({ semu_state.state = (S); semu_state.halt_pc = cpu.pc; semu_state.halt_code = gpr(10); })

#endif
