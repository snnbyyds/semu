#include <device/device.h>
#include <device/timer.h>
#include <utils/timer.h>
#include <utils/state.h>
#include <stdatomic.h>
#include <SDL3/SDL.h>

void init_serial();
void init_timer();
void init_vga();
void init_keyboard();
void init_audio();

void send_keyboard_event(bool keydown, uint32_t keycode);
void update_screen();

void init_timer_intr();

static atomic_bool update_device_signal = false;

// Called in timer thread
static void request_device_update() {
    atomic_store_explicit(&update_device_signal, true, memory_order_relaxed);
}

// This must be called on the main thread
void update_device() {
    if (!atomic_load_explicit(&update_device_signal, memory_order_relaxed)) {
        return;
    }
    update_screen();
    static SDL_Event event;
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
    atomic_store_explicit(&update_device_signal, false, memory_order_relaxed);
}

void init_device() {
    init_timer();
    init_serial();
    init_vga();
    init_keyboard();
    init_audio();

    init_timer_intr();

    // Add the timer callback for device updates
    add_timer_exec(request_device_update);
}
