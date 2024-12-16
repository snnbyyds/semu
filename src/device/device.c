#include <device/device.h>
#include <utils/timer.h>

void init_serial();
void init_timer();
void init_vga();
void update_screen();

void update_device() {
    static uint64_t pre = 0;
    static uint64_t cur = -1;
    cur = get_uptime();
    if (cur - pre > 1000000 / 60) {
        pre = cur;
        update_screen();
    }
}

void init_device() {
    init_vga();
    init_timer();
    init_serial();
}
