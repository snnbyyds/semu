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

void start_gdbstub();
void stop_gdbstub();

void start_sdb();

void start_debugger() {
#ifdef CONFIG_USE_GDBSTUB
    start_gdbstub();
#else
    start_sdb();
#endif
}

void stop_debugger() {
#ifdef CONFIG_USE_GDBSTUB
    stop_gdbstub();
#endif
}
