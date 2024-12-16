#include <device/device.h>
#include <device/mmio.h>
#include <device/timer.h>
#include <utils/timer.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

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

static struct sigevent evp;
static struct itimerspec it;
static timer_t timerid;

#define NR_TIMERS_MAX 8
static void (*subhandlerpool[NR_TIMERS_MAX])(void);
static size_t nr_timers = 0;

static void timer_thread(union sigval v) {
    for (int i = 0; i < nr_timers; i++) {
        subhandlerpool[i]();
    }
}

void add_timer_exec(void (*subhandler)(void)) {
    if (nr_timers == NR_TIMERS_MAX - 1) {
        Error("Can not add more timers!");
        return;
    }
    subhandlerpool[nr_timers++] = subhandler;
    Log("Added timer %p", subhandler);
}

void stop_timers() {
    memset(&it, 0, sizeof(struct itimerspec));
    Log("Stopped timers status: %d", timer_settime(timerid, 0, &it, NULL));
}

void resume_timers() {
    it.it_value.tv_sec = 0;
    it.it_value.tv_nsec = 1000000000 / TIMER_HZ;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 1000000000 / TIMER_HZ;
    Log("Resume timers status: %d", timer_settime(timerid, 0, &it, NULL));
}

void init_timer() {
    timer_base = add_iomap(CONFIG_TIMER_MMIO, 16, timer_io_handler);

    memset(&evp, 0, sizeof(struct sigevent));
    memset(&it, 0, sizeof(struct itimerspec));
    evp.sigev_value.sival_int = 1;
    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = timer_thread;
    timer_create(CLOCK_MONOTONIC, &evp, &timerid);
}
