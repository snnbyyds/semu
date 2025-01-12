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

#include <common.h>
#include <cpu/inst.h>

#ifdef CONFIG_ENABLE_ITRACE
static bool itrace_enable = true;
#endif

void itrace_switch(bool flag) {
#ifdef CONFIG_ENABLE_ITRACE
    itrace_enable = flag;
#endif
}

void itrace(exec_t *restrict info, uint32_t inst) {
#ifdef CONFIG_ENABLE_ITRACE
    extern void disasm(char *s, vaddr_t pc, const void *code, size_t code_size);
    static char buffer[128];
    disasm(buffer, info->pc, &inst, sizeof(uint32_t));
    printf("%s", buffer);
#endif
}
