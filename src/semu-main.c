#include <common.h>

void init_monitor(int argc, char *argv[]);
void start_debugger();

int main(int argc, char *argv[]) {
    init_monitor(argc, argv);
    start_debugger();
    return 0;
}
