#include <utils/state.h>

void init_monitor(int argc, char *argv[]);
void start_debugger();

int main(int argc, char *argv[]) {
    init_monitor(argc, argv);
    start_debugger();
    return is_good_exit_state() ? EXIT_SUCCESS : EXIT_FAILURE;
}
