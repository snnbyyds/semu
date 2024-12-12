#include <common.h>

void init_cpu();
void init_memory();

static void parse_args(int argc, char *argv[]) {

}

static void welcome() {
    puts("Welcome to SEMU!\nType 'help' for help.");
}

void init_monitor(int argc, char *argv[]) {
    parse_args(argc, argv);
    init_cpu();
    init_memory();
    welcome();
}
