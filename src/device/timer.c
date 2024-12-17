#include <device/device.h>
#include <device/mmio.h>
#include <device/timer.h>
#include <utils/timer.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static uint32_t *timer_base = NULL;

enum {
    reg_uptime_lo,
    reg_uptime_hi,
    reg_rtc_sec,
    reg_rtc_min,
    reg_rtc_hour,
    reg_rtc_mday,
    reg_rtc_mon,
    reg_rtc_year,
    reg_epoch_usec,
    reg_epoch_sec_lo,
    reg_epoch_sec_hi,
    nr_regs
};

static void update_uptime() {
    uint64_t uptime = get_uptime();
    timer_base[reg_uptime_lo] = (uint32_t)uptime;
    timer_base[reg_uptime_hi] = (uint32_t)(uptime >> 32);
}

static void update_rtc() {
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = gmtime(&rawtime);
    timer_base[reg_rtc_sec] = timeinfo->tm_sec;
    timer_base[reg_rtc_min] = timeinfo->tm_min;
    timer_base[reg_rtc_hour] = timeinfo->tm_hour;
    timer_base[reg_rtc_mday] = timeinfo->tm_mday;
    timer_base[reg_rtc_mon] = timeinfo->tm_mon + 1;
    timer_base[reg_rtc_year] = timeinfo->tm_year + 1900;

    struct timeval now;
    gettimeofday(&now, NULL);
    timer_base[reg_epoch_usec] = (uint32_t)now.tv_usec;
    timer_base[reg_epoch_sec_lo] = (uint32_t)now.tv_sec;
    timer_base[reg_epoch_sec_hi] = (uint32_t)(now.tv_sec >> 32);
}

static void timer_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    Assert(offset >= 0 && offset <= 28 && !(offset & 0x3));
    if (mmio_rw_op == MMIO_READ) {
        switch (offset) {
            case 4: update_uptime(); break;
            case 8: update_rtc(); break;
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
    extern uint64_t running_seconds;
    static uint64_t triggers = 0;
    for (int i = 0; i < nr_timers; i++) {
        subhandlerpool[i]();
    }
    triggers++;
    if (triggers % TIMER_HZ == 0) {
        running_seconds++;
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
    timer_settime(timerid, 0, &it, NULL);
}

void resume_timers() {
    it.it_value.tv_sec = 0;
    it.it_value.tv_nsec = 1000000000 / TIMER_HZ;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_nsec = 1000000000 / TIMER_HZ;
    timer_settime(timerid, 0, &it, NULL);
}

void init_timer() {
    timer_base = add_iomap(CONFIG_TIMER_MMIO, 44, timer_io_handler);

    memset(&evp, 0, sizeof(struct sigevent));
    memset(&it, 0, sizeof(struct itimerspec));
    evp.sigev_value.sival_int = 1;
    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = timer_thread;
    timer_create(CLOCK_MONOTONIC, &evp, &timerid);
}
