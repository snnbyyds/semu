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

#ifndef __JIT_COMMON_RULE_H__
#define __JIT_COMMON_RULE_H__

#include <cpu/inst.h>
#include <utils/stack.h>

#define HANDLE_INTR \
    "HANDLE_INTR();\n"

#define TEMPLATE \
    "__%08x: {\n" \
    HANDLE_INTR \
    "   %s\n" \
    "   npc = 0x%08x;\n" \
    "   goto __%08x;\n" \
    "}\n"

#define APPLY_TEMPLATE(pc, npc, exec_str) \
    ({ \
        sprintf(buffer, TEMPLATE, pc, exec_str, npc, npc); \
    })

#define ADD_NEXT(next_pc) \
    ({ \
        stack32_push(stack, next_pc); \
    })

#endif
