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

#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <interpreter.h>
#include <jit.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <utils/snapshot.h>
#include <string.h>

typedef struct {
    const char *name;
    const char *desc;
    bool (*handler)(char *);
} sdb_cmd_t;

/* sdb builtin commands */

static bool semu_quit = false;

static bool cmd_clear(char *args) {
    return (bool)(!system("clear"));
}

static bool cmd_help(char *args);

static bool cmd_quit(char *args) {
    puts("Exiting SEMU...\nHave a nice day!");
    semu_quit = true; // TODO: Call our status handling plugin
    return true;
}

static bool cmd_si(char *args) {
    uint64_t step;
    if (!args || !(*args)) {
        step_interpreter_exec(1);
        return true;
    }
    sscanf(args, "%" SCNu64 "", &step);
    step_interpreter_exec(step);
    return true;
}

static bool cmd_c(char *args) {
    step_interpreter_exec(-1);
    return true;
}

#ifdef CONFIG_PAUSE_PC
static bool cmd_pause(char *args) {
    extern uint64_t sdb_pause_pc;
    if (!args && !(*args)) {
        Warn("Invalid argument passed!");
        return false;
    }
    sscanf(args, "%" SCNx64 "", &sdb_pause_pc);
    Info("Will pause at PC: 0x%08" PRIx64 "", sdb_pause_pc);
    return true;
}
#endif

static bool cmd_info(char *args) {
    if (args && !strcmp(args, "r")) {
        reg_val_display();
        putchar('\n');
        fpr_val_display();
    } else {
        Warn("Invalid argument passed!");
        return false;
    }
    return true;
}

static bool cmd_save(char *args) {
    if (!args || !(*args)) {
        Warn("Invalid argument passed!");
        return false;
    }
    save_snapshot(args);
    return true;
}

static bool cmd_load(char *args) {
    if (!args || !(*args)) {
        Warn("Invalid argument passed!");
        return false;
    }
    load_snapshot(args);
    return true;
}

#ifdef CONFIG_ENABLE_ITRACE
static bool cmd_itrace(char *args) {
    extern void itrace_switch(bool);
    if (!args || !(*args)) {
        Warn("Invalid argument passed!");
        return false;
    }
    itrace_switch(!strncmp(args, "on", 2));
    return true;
}
#endif

static sdb_cmd_t cmdtbl[] = {
    {"clear", "Clear the screen", cmd_clear},
    {"quit", "Quit SN Emu", cmd_quit},
    {"si", "Single step", cmd_si},
    {"c", "Continue", cmd_c},
#ifdef CONFIG_PAUSE_PC
    {"pause", "Pause at a certain pc", cmd_pause},
#endif
    {"info", "Use 'info r' for reg vals", cmd_info},
    {"save", "Save snapshot", cmd_save},
    {"load", "Load snapshot", cmd_load},
#ifdef CONFIG_ENABLE_ITRACE
    {"itrace", "Switch itrace state", cmd_itrace},
#endif
    {"help", "Print help msg", cmd_help}
};

static bool cmd_help(char *args) {
    for (size_t i = 0; i < LENGTH(cmdtbl); i++) {
        printf("%-6s %-10s\n", cmdtbl[i].name, cmdtbl[i].desc);
    }
    return true;
}

/* sdb main session */

static bool batch_mode = false;
static char *input = NULL;

static inline void sdb_read_input() {
    if (input) {
        free(input); // free previous input
        input = NULL;
    }
    input = readline("\033[32m(semu) \033[0m");
    if (input && *input) {
        add_history(input);
    }
}

static inline void sdb_handle_input() {
    if (input == NULL || *input == '\0') {
        return;
    }
    char *cmd = strtok(input, " ");
    if (!cmd) {
        return;
    }
    char *args = strtok(NULL, " ");
    size_t i = 0;
    for (; i < LENGTH(cmdtbl); i++) {
        if (!strcmp(cmdtbl[i].name, cmd)) {
            if (cmdtbl[i].handler(args)) {
                break;
            } else {
                Warn("sdb cmd %p failed.", cmdtbl[i].handler);
                break;
            }
        }
    }
    if (i >= LENGTH(cmdtbl)) {
        puts("Invalid cmd");
        cmd_help(NULL);
    }
}

static void sdb_main_session() {
    using_history();
    while (1) {
        sdb_read_input();
        sdb_handle_input();
        if (semu_quit) {
            break;
        }
    }
}

void sdb_batchmode_on() {
    batch_mode = true;
}

void start_sdb() {
    if (batch_mode) {
        // cmd_c(NULL);
        jit_run();
        return;
    }
    sdb_main_session();
}

void exit_sdb() {
    if (!input) {
        free(input);
    }
}
