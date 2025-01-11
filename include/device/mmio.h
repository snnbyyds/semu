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

#ifndef __MMIO_H__
#define __MMIO_H__

#include <common.h>

#define MMIO_WRITE true
#define MMIO_READ false

typedef bool mmio_rw_t;
typedef void (*io_handler_t) (mmio_rw_t mmio_rw_op, size_t offset, size_t len);

typedef struct {
    ioaddr_t low;
    ioaddr_t high;
    io_handler_t io_handler;
    void *space;
} IOMap;

void *add_iomap(ioaddr_t start, size_t size, io_handler_t handler);
word_t mmio_read(ioaddr_t addr, size_t len);
void mmio_write(ioaddr_t addr, size_t len, word_t data);
void init_mmio();

#endif
