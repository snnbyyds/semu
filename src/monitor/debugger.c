#include <common.h>

void start_gdbstub();
void start_sdb();

void start_debugger() {
#ifdef CONFIG_USE_GDBSTUB
    start_gdbstub();
#else
    start_sdb();
#endif
}
