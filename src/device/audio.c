#include <device/mmio.h>
#include <SDL3/SDL.h>
#include <string.h>

#define sbuf_size ((uint32_t)CONFIG_SB_SIZE)

enum {
    reg_freq,
    reg_channels,
    reg_samples,
    reg_sbuf_size,
    reg_init,
    reg_count,
    nr_regs
};

enum { audio_uninit, audio_init, audio_config };

static uint32_t *audio_base = NULL;
static uint8_t *sbuf = NULL;

static inline void __audio_callback(void *userdata, uint8_t *stream, int len) {
#define TO_PLAY audio_base[reg_count]

    static uint32_t sbuf_idx = 0;
    if (TO_PLAY == 0) {
        memset(stream, 0, len);
        return;
    }
    int nread = len;
    if (TO_PLAY < len) {
        nread = TO_PLAY;
    }
    for (uint32_t i = 0; i < nread; i++) {
        stream[i] = sbuf[(sbuf_idx + i) % sbuf_size];
    }
    if (nread < len) {
        memset(stream + nread, 0, len - nread);
    }
    sbuf_idx = (sbuf_idx + nread) % sbuf_size;
    TO_PLAY -= len;
}

static void audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
    if (additional_amount > 0) {
        uint8_t *data = SDL_stack_alloc(uint8_t, additional_amount);
        if (data) {
            __audio_callback(userdata, data, additional_amount);
            SDL_PutAudioStreamData(stream, data, additional_amount);
            SDL_stack_free(data);
        }
    }
}

static void audio_play() {
    if (audio_base[reg_init] != audio_config) {
        return;
    }
    const SDL_AudioSpec desired_spec = {
        .format = SDL_AUDIO_S16,
        .channels = audio_base[reg_channels],
        .freq = audio_base[reg_freq]
    };
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired_spec, audio_callback, NULL);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
}

static void audio_io_handler(mmio_rw_t mmio_rw_op, size_t offset, size_t len) {
    Assert((offset & 0x3) == 0 && offset < sizeof(uint32_t) * nr_regs);
    if (mmio_rw_op == MMIO_WRITE) {
        uint32_t reg_no = offset >> 2;
        switch (reg_no) {
            case reg_init: audio_play(); break;
            case reg_freq: case reg_channels: case reg_samples: /* Fallthrough */
            case reg_count: break;
            default: Error("Unhandled MMIO write!"); break;
        }
    }
}

void init_audio() {
#define AUDIO_CTL_MMIO_SIZE (nr_regs * sizeof(uint32_t))
    audio_base = add_iomap(CONFIG_AUDIO_CTL_MMIO, AUDIO_CTL_MMIO_SIZE, audio_io_handler);
    sbuf = add_iomap(CONFIG_SB_ADDR, CONFIG_SB_SIZE, NULL);
    memset(audio_base, 0, AUDIO_CTL_MMIO_SIZE);
    audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
#undef AUDIO_CTL_MMIO_SIZE
}
