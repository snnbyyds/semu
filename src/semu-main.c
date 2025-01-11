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

#include <device/device.h>
#include <utils/state.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

void init_monitor(int argc, char *argv[]);
void start_debugger();
void close_disasm();

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    init_monitor(argc, argv);
    start_debugger();
    close_device();
#ifdef CONFIG_ENABLE_ITRACE
    close_disasm();
#endif
    return is_good_exit_state() ? EXIT_SUCCESS : EXIT_FAILURE;
}
