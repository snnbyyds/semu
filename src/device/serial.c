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

#include <device/mmio.h>

uint8_t *serial_base = NULL;

static void serial_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    Assert(len == 1 && offset == 0);
    Assert(serial_base);
    if (mmio_rw_op == MMIO_WRITE) {
        putchar(*serial_base);
    } else {
        Error("Read not implemented!");
    }
}

void init_serial() {
    serial_base = add_iomap(CONFIG_SERIAL_MMIO, 8, serial_io_handler);
}
