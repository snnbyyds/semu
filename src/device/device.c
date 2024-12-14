#include <device/device.h>

void init_serial();
void init_timer();

void init_device() {
    init_timer();
    init_serial();
}
