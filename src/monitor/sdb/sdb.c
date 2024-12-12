#include <common.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

typedef struct {
    const char *name;
    const char *desc;
    bool (*handler)(char *);
} sdb_cmd_t;

/* sdb builtin commands */

static bool cmd_clear(char *args) {
    return (bool)(!system("clear"));
}

static bool cmd_help(char *args);

static bool cmd_quit(char *args) {
    puts("Exiting SEMU...\nHave a nice day!");
    exit(EXIT_SUCCESS); // TODO: Call our status handling plugin
    return false;
}

static sdb_cmd_t cmdtbl[] = {
    {"clear", "Clear the screen", cmd_clear},
    {"quit", "Quit SN Emu", cmd_quit},
    {"help", "Print help msg", cmd_help}
};

static bool cmd_help(char *args) {
    for (size_t i = 0; i < LENGTH(cmdtbl); i++) {
        printf("name: %s\tdesc: %s\n", cmdtbl[i].name, cmdtbl[i].desc);
    }
    return true;
}

/* sdb main session */

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
    char *args = cmd + strlen(cmd) + 1;
    if (*(args - 1) == '\0') {
        args = NULL;
    }
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
    }
}

void start_sdb() {
    Log("Entering sdb session...");
    sdb_main_session();
}

void exit_sdb() {
    if (!input) {
        free(input);
    }
}
