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

#include <cpu/jit.h>
#include <memory.h>
#include <utils/set.h>
#include <utils/stack.h>

static inline void reset_code_block(code_block_t *block) {
    if (block->c_code) {
        free(block->c_code);
    }
    block->len = 0;
}

static void append_code_block(code_block_t *block, const char *str) {
    size_t strsize = strlen(str) + 1;
    if (unlikely(!block->c_code)) {
        block->c_code = (char *)malloc(strsize);
        block->len = strsize - 1;
        strcpy(block->c_code, str);
        return;
    }
    assert(block->len);
    block->c_code = (char *)realloc(block->c_code, block->len + strsize);
    assert(!block->c_code[block->len]);
    strcpy(block->c_code + block->len, str);
    block->len += (strsize - 1);
}

code_block_t *jit_gen_c_code(vaddr_t start_pc) {
    static code_block_t block = CODE_BLOCK_INITIALIZER;
    static char buffer[256];
    static stack32_t stack;
    static set32_t set;

    reset_code_block(&block);
    stack32_init(&stack);
    set32_reset(&set);

    stack32_push(&stack, start_pc);
    vaddr_t pc = -1;
    while ((pc = stack32_pop(&stack)) != -1) {
        if (!set32_add(&set, pc)) {
            continue;
        }
        sprintf(buffer, "__%08x: {\n", pc);
        append_code_block(&block, buffer);
        //const uint32_t inst = vaddr_ifetch(pc);

        /* TODO: Decode inst */
    }
    /* TODO */
    return NULL;
}
