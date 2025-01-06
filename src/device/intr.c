#include <device/timer.h>
#include <cpu/cpu.h>

void dev_raise_intr() {
    cpu.intr = true;
}
