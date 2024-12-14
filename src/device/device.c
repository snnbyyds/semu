#include <device/device.h>
#include <utils/timer.h>
#include <utils/state.h>
#include <SDL3/SDL.h>

void init_serial();
void init_timer();
void init_vga();
void update_screen();

void update_device() {
    static uint64_t pre = 0;
    static uint64_t cur = -1;
    static SDL_Event event;

    cur = get_uptime();
    if (cur - pre > 1000000 / 60) {
        pre = cur;
        update_screen();
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: Log("Received SDL quit event!"); SET_STATE(QUIT); break;
                default: break;
            }
        }
    }
}

void init_device() {
    init_vga();
    init_timer();
    init_serial();
}
