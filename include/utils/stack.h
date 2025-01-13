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

#ifndef __STACK_H__
#define __STACK_H__

#include <common.h>

#define STACK_CAP 256

typedef struct {
    uint32_t elements[STACK_CAP];
    int top;
} stack32_t;

__attribute__((always_inline))
static inline void stack32_push(stack32_t *stack, uint32_t element) {
    assert(stack->top < STACK_CAP);
    for (int i = 0; i < stack->top; i++) {
        if (element == stack->elements[i]) {
            return;
        }
    }
    stack->elements[stack->top++] = element;
}

__attribute__((always_inline))
static inline uint32_t stack32_pop(stack32_t *stack) {
    if (stack->top == 0) {
        return -1;
    }
    return stack->elements[--stack->top];
}

__attribute__((always_inline))
static inline void stack32_init(stack32_t *stack) {
    stack->top = 0;
}

#endif
