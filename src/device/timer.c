#include <device/mmio.h>
#include <utils/timer.h>

static uint32_t *timer_base = NULL;

static void update_uptime() {
    uint64_t uptime = get_uptime();
    timer_base[0] = (uint32_t)uptime;
    timer_base[1] = (uint32_t)(uptime >> 32);
}

static void timer_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    Assert(offset >= 0 && offset <= 12 && !(offset & 0x3));
    if (mmio_rw_op == MMIO_READ) {
        switch (offset) {
            case 4: update_uptime();
            default: break;
        }
    }
}

void init_timer() {
    timer_base = add_iomap(CONFIG_TIMER_MMIO, 16, timer_io_handler);
}
