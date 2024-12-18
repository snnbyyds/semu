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
void init_disk();

void close_disk();

void fetch_keyboard_status();
void update_screen();

static atomic_bool update_device_signal = false;

// Called in timer thread
static void request_device_update() {
    atomic_store_explicit(&update_device_signal, true, memory_order_relaxed);
}

// This must be called on the main thread
__attribute__((always_inline))
void update_device() {
    if (!atomic_load_explicit(&update_device_signal, memory_order_relaxed)) {
        return;
    }
    update_screen();
    fetch_keyboard_status();
    atomic_store_explicit(&update_device_signal, false, memory_order_relaxed);
}

void init_device() {
    init_timer();
    init_serial();
    init_vga();
    init_keyboard();
    init_audio();
    init_disk();

    // Add the timer callback for device updates
    add_timer_exec(request_device_update);
}

void close_device() {
    close_disk();
    SDL_Quit();
}
