#include <device/device.h>
#include <utils/timer.h>
#include <utils/state.h>
#include <SDL3/SDL.h>

void init_serial();
void init_timer();
void init_vga();
void init_keyboard();
void init_audio();

void update_screen();
void send_keyboard_event(bool keydown, uint32_t keycode);

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
                case SDL_EVENT_KEY_DOWN:
                    send_keyboard_event(true, event.key.scancode);
                    break;
                case SDL_EVENT_KEY_UP:
                    send_keyboard_event(false, event.key.scancode);
                    break;
                case SDL_EVENT_QUIT:
                    Log("Received SDL quit event!");
                    SET_STATE(QUIT);
                    break;
                default:
                    break;
            }
        }
    }
}

void init_device() {
    init_timer();
    init_serial();
    init_vga();
    init_keyboard();
    init_audio();
}
