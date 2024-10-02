/**
 * @file arcade.c
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../../include/arcade/arcade.h"
#include "../../include/arcade/private/arcade.h"

Arcade* arcade_init(const char* arcade_name, const char* rom_folder_path, int screen_width, int screen_height)
{
    Arcade* arcade = (Arcade*) malloc(sizeof(Arcade));

    // Reset running flag
    arcade->running = false;

    // Store arcade name
    arcade->name = arcade_name;

    // Init CPU
    arcade_init_cpu(arcade, rom_folder_path);

    // Init shift register
    arcade_init_shift_register(arcade);

    // Init display
    arcade_init_display(arcade, screen_width, screen_height);

    return arcade;
}

void arcade_init_display(Arcade* arcade, int screen_width, int screen_height)
{
    arcade->display.width = screen_width;
    arcade->display.height = screen_height;
}

void arcade_free(Arcade* a)
{
    cpu8080_free(a->cpu);
    free(a);
}

void arcade_start(Arcade* arcade)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    // Create SDL elements
    SDL_Window* window = SDL_CreateWindow(
        arcade->name,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (window == NULL)
    {
        printf("window could not be created: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (renderer == NULL)
    {
        printf("renderer could not be created: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        arcade->display.width,
        arcade->display.height
    );

    Uint32 px_format;
    SDL_QueryTexture(texture, &px_format, NULL, NULL, NULL);
    SDL_PixelFormat* pixel_format = SDL_AllocFormat(px_format);

    // Set running and create associated threads
    arcade->running = true;
    SDL_Thread* tCPU = SDL_CreateThread(arcade_emulate_cpu, "CPU", arcade);
    SDL_Thread* tVideoInterrupt = SDL_CreateThread(arcade_emulate_video_interruptions, "VBL", arcade);

    arcade_update_display(arcade, window, renderer, texture, pixel_format);

    // Display variables
    Uint32 frame_update_period_ms =  1e+3 / 30;  // 30 FPS
    Uint32 last_upate_time = SDL_GetTicks();
    int sleep_time;

    // Event loop
    SDL_Event event;
    bool quit = false;
    
    while (!quit)
    {
        // Handle all user events
        while (SDL_PollEvent(&event))
        {
            arcade_handle_joystick_event(arcade, &event, &quit);
        }

        // Sleep until next update time
        sleep_time = frame_update_period_ms - (SDL_GetTicks() - last_upate_time);
        if (sleep_time > 0)
            SDL_Delay(sleep_time);
        last_upate_time = SDL_GetTicks();

        // Update screen
        arcade_update_display(arcade, window, renderer, texture, pixel_format);
    }

    SDL_FreeFormat(pixel_format);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    arcade->running = false;
    SDL_WaitThread(tVideoInterrupt, NULL);
    SDL_WaitThread(tCPU, NULL);
}