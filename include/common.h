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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#include <debug.h>
#include <generated/autoconf.h>
#include <macro.h>

#include <softfloat.h>

#ifndef VERSION
#define VERSION 0.0
#endif

typedef uint32_t word_t;
typedef int32_t sword_t;

typedef word_t paddr_t;
typedef paddr_t vaddr_t;
typedef word_t ioaddr_t;

#define PRIword PRIu32
#define PRIaddr PRIx32

#define PGSIZE 4096
#define PG_ALIGN __attribute((aligned(PGSIZE)))

#define __EXPORT __attribute__((visibility("default")))

#endif
