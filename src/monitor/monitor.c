#include <common.h>

void init_cpu();
void init_memory();

static void parse_args(int argc, char *argv[]) {

}

static void welcome() {
    printf("\033[35;47mWelcome to SEMU!\033[0m\n");
    printf("Type '\033[32mhelp\033[0m' for help.\n");
}

void init_monitor(int argc, char *argv[]) {
    parse_args(argc, argv);
    init_cpu();
    init_memory();
    welcome();
}
