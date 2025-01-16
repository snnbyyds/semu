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

#include <utils/difftest.h>
#include <utils/state.h>

__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
    assert(0);
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
    assert(0);
}

__EXPORT void difftest_exec(uint64_t n) {
    assert(0);
}

__EXPORT void difftest_raise_intr(word_t NO) {
    assert(0);
}

__EXPORT void difftest_init(int port) {
    assert(0);
}