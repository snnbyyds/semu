#include <common.h>
#include <sys/time.h>

static uint64_t boot_time = -1;

uint64_t get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000000 + t.tv_usec;
}

uint64_t get_uptime() {
    return get_time() - boot_time;
}

void get_boot_time() {
    boot_time = get_uptime();
}
