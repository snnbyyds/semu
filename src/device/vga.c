#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <device/mmio.h>
#include <common.h>

#define SCREEN_W 400
#define SCREEN_H 300
#define SCREEN_SIZE SCREEN_W * SCREEN_H * sizeof(uint32_t)

enum { reg_ctldata, reg_sync };

static SDL_Renderer* renderer = NULL;
static SDL_Texture *texture = NULL;

static void init_screen() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SEMU ( CPL2024 Special release )", SCREEN_W * 3, SCREEN_H * 3, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, NULL);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_TARGET | SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    SDL_RenderPresent(renderer);
}

static void *fb = NULL;
static uint32_t *vga_base = NULL;

void update_screen() {
    if (vga_base[reg_sync]) {
        SDL_UpdateTexture(texture, NULL, fb, SCREEN_W * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        vga_base[reg_sync] = 0;
    }
}

void init_vga() {
    init_screen();
    vga_base = add_iomap(CONFIG_VGA_MMIO, 8, NULL);
    fb = add_iomap(CONFIG_FB_ADDR, SCREEN_SIZE, NULL);
    vga_base[reg_ctldata] = (SCREEN_W << 16) | SCREEN_H;
    vga_base[reg_sync] = 0;
}
