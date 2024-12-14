#include <device/mmio.h>
#include <sys/time.h>

static uint64_t boot_time = -1;
static uint32_t *timer_base = NULL;

static inline uint64_t get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000 + t.tv_usec;
}

static void update_uptime() {
    assert(boot_time != -1);
    uint64_t uptime = get_time() - boot_time;
    timer_base[0] = (uint32_t)uptime;
    timer_base[1] = (uint32_t)(uptime >> 32);
}

static void timer_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    assert(offset >= 0 && offset <= 12 && !(offset & 0x3));
    if (mmio_rw_op == MMIO_READ) {
        switch (offset) {
            case 4: update_uptime();
            default: break;
        }
    }
}

void init_timer() {
    if (boot_time == -1) {
        boot_time = get_time();
    }
    timer_base = add_iomap(CONFIG_TIMER_MMIO, 16, timer_io_handler);
}
