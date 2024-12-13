#include <cpu/cpu.h>
#include <utils/state.h>

void init_cpu(bool img_builtin);
void init_memory();

static char *image = NULL;

static void parse_args(int argc, char *argv[]) {
}

static void welcome() {
    printf("\033[35;47mWelcome to SEMU!\033[0m\n");
    printf("Type '\033[32mhelp\033[0m' for help.\n");
}

static void load_image() {
    assert(!image); // TODO
}

void init_monitor(int argc, char *argv[]) {
    parse_args(argc, argv);
    load_image();
    SET_STATE(STOP);
    init_memory();
    init_cpu(image == NULL);
    welcome();
}
