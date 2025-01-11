/*
 * Copyright 2025 Nuo Shen, Nanjing University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3_image/SDL_image.h>
#include <device/mmio.h>
#include <device/timer.h>
#include <common.h>

#define SCREEN_W 400
#define SCREEN_H 300
#define SCREEN_SIZE SCREEN_W * SCREEN_H * sizeof(uint32_t)

enum { reg_ctldata, reg_sync };

static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static SDL_Window *window = NULL;

static inline void load_window_icon() {
    assert(window);
    SDL_IOStream *stream = SDL_IOFromFile("resource/logo.png", "r");
    SDL_Surface *iconSurface = IMG_LoadPNG_IO(stream);
    SDL_SetWindowIcon(window, iconSurface);
    SDL_DestroySurface(iconSurface);
}

static inline void semu_bootanim() {
    SDL_IOStream *stream = SDL_IOFromFile("resource/splash.jpg", "r");
    SDL_Surface *imageSurface = IMG_LoadJPG_IO(stream);
    SDL_Texture *imageTexture = SDL_CreateTextureFromSurface(renderer, imageSurface);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    const SDL_FRect destRect = {0, 0, SCREEN_W * 3, SCREEN_H * 3};
    SDL_RenderTexture(renderer, imageTexture, NULL, &destRect);
    SDL_RenderPresent(renderer);
    SDL_Delay(512);

    SDL_DestroySurface(imageSurface);
    SDL_DestroyTexture(imageTexture);
    SDL_CloseIO(stream);
}

static void init_screen() {
    window = SDL_CreateWindow(
        "SEMU ( CPL2024 Special release )",
        SCREEN_W * 3,
        SCREEN_H * 3,
        SDL_WINDOW_RESIZABLE
    );
    load_window_icon();

    renderer = SDL_CreateRenderer(window, NULL);
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_TARGET | SDL_TEXTUREACCESS_STREAMING,
        SCREEN_W, SCREEN_H
    );
    semu_bootanim();
}

static void *fb = NULL;
static uint32_t *vga_base = NULL;

// This must be called on the main thread
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
