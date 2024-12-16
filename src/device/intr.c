#include <cpu/cpu.h>
#include <device/timer.h>

static void dev_raise_intr() {
    cpu.intr = true;
}

void init_timer_intr() {
    add_timer_exec(dev_raise_intr);
}
