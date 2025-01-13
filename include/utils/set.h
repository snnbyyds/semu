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

#ifndef __SET_H__
#define __SET_H__

#include <common.h>
#include <string.h>

#include "hash.h"

#define SET_SIZE (32 * 1024)

typedef struct {
    uint32_t table[SET_SIZE];
} set32_t;

__attribute__((always_inline))
static inline bool set32_contains(set32_t *set, uint32_t element) {
    assert(element);
    uint64_t idx = hash(element, SET_SIZE);
    while (set->table[idx]) {
        if (set->table[idx] == element) {
            return true;
        }
        idx = (idx + 1) % SET_SIZE;
    }
    return false;
}

__attribute__((always_inline))
static inline bool set32_add(set32_t *set, uint32_t element) {
    assert(element);
    uint64_t idx = hash(element, SET_SIZE);
    while (set->table[idx]) {
        if (set->table[idx] == element) {
            return false;
        }
        idx = (idx + 1) % SET_SIZE;
    }
    set->table[idx] = element;
    return true;
}

__attribute__((always_inline))
static inline void set32_reset(set32_t *set) {
    memset(set, 0, sizeof(set32_t));
}

#endif
