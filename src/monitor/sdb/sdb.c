#include <cpu/cpu.h>
#include <cpu/reg.h>
#include <readline/readline.h>
#include <readline/history.h>
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
        cpu_exec(1);
        return true;
    }
    sscanf(args, "%" PRIu64 "", &step);
    cpu_exec(step);
    return true;
}

static bool cmd_c(char *args) {
    cpu_exec(-1);
    return true;
}

static bool cmd_info(char *args) {
    if (args && !strcmp(args, "r")) {
        reg_val_display();
    } else {
        Warn("Invalid argument passed!");
    }
    return true;
}

static sdb_cmd_t cmdtbl[] = {
    {"clear", "Clear the screen", cmd_clear},
    {"quit", "Quit SN Emu", cmd_quit},
    {"si", "Single step", cmd_si},
    {"c", "Continue", cmd_c},
    {"info", "Use 'info r' for reg vals", cmd_info},
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
        cmd_c(NULL);
    }
    sdb_main_session();
}

void exit_sdb() {
    if (!input) {
        free(input);
    }
}
