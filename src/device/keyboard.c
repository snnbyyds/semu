#include <SDL3/SDL.h>
#include <device/device.h>
#include <device/mmio.h>
#include <utils/state.h>

#define SEMU_KEYS(_) \
    _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
    _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
    _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
    _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
    _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
    _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
    _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)

#define SEMU_KEY_NAMES(key) SEMU_KEY_ ## key,

typedef enum {
    SEMU_KEY_NONE = 0,
    SEMU_KEYS(SEMU_KEY_NAMES)
    NR_KEYS
} SEMU_KEYCODE;

// map sdl_keycode to semucode
#define KEY_MAP(key) [SDL_SCANCODE_ ## key] = SEMU_KEY_ ## key,

static SEMU_KEYCODE semu_keymapper[] = {
    SEMU_KEYS(KEY_MAP)
};

static uint32_t keyeventqueue[NR_KEYS];
static size_t queue_left = 0, queue_right = 0;

#define SEMU_KEYCODE(sdl_keycode) (uint32_t)semu_keymapper[sdl_keycode]
#define SEMU_KEYEVENT(semu_keycode, keydown) ((semu_keycode) | (keydown << KEYDOWN_SHIFT))

static inline void push_keyevent(uint32_t semu_keyevent) {
    keyeventqueue[queue_right] = semu_keyevent;
    queue_right = (queue_right + 1) % NR_KEYS;
    if (queue_right == queue_left) {
        Error("keyeventqueue OVERFLOW!");
    }
}

static inline uint32_t pop_keyevent() {
    if (queue_left == queue_right) {
        return SEMU_KEY_NONE;
    }
    uint32_t ret = keyeventqueue[queue_left];
    queue_left = (queue_left + 1) % NR_KEYS;
    return ret;
}

static void send_keyboard_event(bool keydown, uint32_t sdl_keycode) {
    SEMU_KEYCODE semu_keycode = SEMU_KEYCODE(sdl_keycode);
    if (semu_state.state == RUNNING && semu_keycode != SEMU_KEY_NONE) {
        push_keyevent(SEMU_KEYEVENT(semu_keycode, keydown));
    }
}

void fetch_keyboard_status() {
    static SDL_Event event;
    while (SDL_PollEvent(&event)) {
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

static uint32_t *kbd_base = NULL;

static void kbd_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    if (mmio_rw_op == MMIO_READ) {
        *kbd_base = pop_keyevent();
    }
}

void init_keyboard() {
    kbd_base = add_iomap(CONFIG_KBD_MMIO, 4, kbd_io_handler);
    kbd_base[0] = 0;
}
