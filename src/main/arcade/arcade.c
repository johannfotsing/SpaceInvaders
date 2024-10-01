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
#include "../../include/arcade.h"
#include "../../include/private/arcade.h"

Arcade* arcade_init(const char* arcade_name, const char* rom_folder_path, int screen_width, int screen_height)
{
    Arcade* arcade = (Arcade*) malloc(sizeof(Arcade));

    // Reset running flag
    arcade->running = false;

    // Store arcade name
    arcade->name = arcade_name;

    // Init CPU
    arcade_init_cpu(arcade, rom_folder_path);

    // Init display
    arcade_init_display(arcade, screen_width, screen_height);

    return arcade;
}

void arcade_init_display(Arcade* arcade, int screen_width, int screen_height)
{
    arcade->display.width = screen_width;
    arcade->display.height = screen_height;
}

void arcade_init_cpu(Arcade* arcade, const char* rom_folder_path)
{
    // CPU
    // 4 input ports
    // 8 output ports
    // 16kB of memory
    // 2 MHz frequency
    Cpu8080Config cpu_config = {4, 8, 16, 2.e+6};
    arcade->cpu = cpu8080_init(&cpu_config);
    
    /// Load SpaceInvaders ROM files
    size_t len = strlen(rom_folder_path);
    size_t copy_len = rom_folder_path[len-1] == '/' ? len - 1 : len;
    const char* first_rom_name = "/invaders";
    size_t rom_filename_len = strlen(first_rom_name);
    size_t file_path_len = copy_len + rom_filename_len;
    char* rom_file_path = (char*) malloc(file_path_len + 1);
    rom_file_path = strcpy(rom_file_path, rom_folder_path);
    rom_file_path = strcat(rom_file_path, first_rom_name);
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x0000);

    free(rom_file_path);

    // Configure program counter for RAM in memory
    arcade->cpu->program_counter = 0x0000;

    // Shift register init
    cpu8080_register_output_callback(arcade->cpu, &on_shift_data_available, SHFT_AMNT);
}

void arcade_free(Arcade* a)
{
    cpu8080_free(a->cpu);
    free(a);
}

void on_shift_data_available(Cpu8080* cpu)
{
    static uint16_t shift_register = 0x0000;
    uint16_t shift_data;
    cpu8080_read_port(cpu, SHFT_DATA, (uint8_t*) &shift_data);
    shift_register = ((shift_register >> 8) & 0x00ff) | ((shift_data << 8) & 0xff00);
    uint8_t shift_offset;
    cpu8080_read_port(cpu, SHFT_AMNT, &shift_offset);
    shift_offset &= 0b111; // offset is 3 bits long max
    uint8_t shift_result = ((shift_register << shift_offset) >> 8) & 0xff;
    cpu8080_write_port(cpu, SHFT_IN, shift_result);
}

void arcade_update_display(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format)
{
    int pitch;
    Uint32* texture_pixels;
    SDL_LockTexture(texture, NULL, (void**) &texture_pixels, &pitch);
    for (int byte_pixel_index = 0; byte_pixel_index < arcade->display.width * arcade->display.height / 8; ++byte_pixel_index)
    {
        // Read pixel values
        uint8_t image_byte;
        cpu8080_read_membyte(arcade->cpu, 0x2000 + byte_pixel_index, &image_byte);

        // Update texture
        for (int pixel_index = byte_pixel_index * 8; pixel_index < (byte_pixel_index + 1) * 8; ++pixel_index)
        {
            // Turn bit pixel into color pixel
            int bit_idx = pixel_index % 8;
            uint8_t pix_val = (image_byte & (1 << bit_idx )) * 255;
            // Rotate 90 degrees
            int mem_x, mem_y, texture_x, texture_y, texture_index;
            mem_x = pixel_index % arcade->display.height;
            mem_y = pixel_index / arcade->display.height;
            texture_x = mem_y;
            texture_y = (arcade->display.height - 1) - mem_x;
            texture_index = texture_y * arcade->display.width + texture_x;
            //*
            texture_pixels[texture_index] = SDL_MapRGB(px_format, pix_val, pix_val, pix_val);
        }
    }
    SDL_UnlockTexture(texture);

    // Update renderer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Update window image
    SDL_RenderPresent(renderer);
}

void arcade_handle_event(Arcade* a, const SDL_Event* event, bool* quit)
{
    if (event->type == SDL_QUIT)
        *quit = true;

    else if (event->type == SDL_KEYDOWN)
    {
        switch(event->key.keysym.scancode)
        {
        case SDL_SCANCODE_C: on_coin_pressed(a); break;
        // P1
        case SDL_SCANCODE_UP: on_P1_shoot_pressed(a); break;
        case SDL_SCANCODE_DOWN: on_P1_start_pressed(a); break;
        case SDL_SCANCODE_LEFT: on_P1_left_pressed(a); break;
        case SDL_SCANCODE_RIGHT: on_P1_right_pressed(a); break;
        // P2
        case SDL_SCANCODE_W: on_P2_shoot_pressed(a); break;
        case SDL_SCANCODE_S: on_P2_start_pressed(a); break;
        case SDL_SCANCODE_A: on_P2_left_pressed(a); break;
        case SDL_SCANCODE_D: on_P2_right_pressed(a); break;
        // QUIT
        case SDL_SCANCODE_ESCAPE: *quit = true; break;
        default: break;
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        switch(event->key.keysym.scancode)
        {
        case SDL_SCANCODE_C: on_coin_released(a); break;
        // P1
        case SDL_SCANCODE_UP: on_P1_shoot_released(a); break;
        case SDL_SCANCODE_DOWN: on_P1_start_released(a); break;
        case SDL_SCANCODE_LEFT: on_P1_left_released(a); break;
        case SDL_SCANCODE_RIGHT: on_P1_right_released(a); break;
        // P2
        case SDL_SCANCODE_W: on_P2_shoot_released(a); break;
        case SDL_SCANCODE_S: on_P2_start_released(a); break;
        case SDL_SCANCODE_A: on_P2_start_released(a); break;
        case SDL_SCANCODE_D: on_P2_right_released(a); break;
        // QUIT
        case SDL_SCANCODE_ESCAPE: *quit = true; break;
        default: break;
        }
    }
}

int arcade_emulate_video_interruptions(void* a)
{
    Arcade* arcade = (Arcade*) a;
    uint8_t video_isr = 1;
    Uint32 video_interruption_period_ms = 1e+3 / (30 * 2);
    int sleep_time;
    Uint32 last_video_interruption_time = SDL_GetTicks();

    while (arcade->running)
    {
        sleep_time = video_interruption_period_ms - (SDL_GetTicks() - last_video_interruption_time);
        if (sleep_time > 0)
            SDL_Delay(sleep_time);

        last_video_interruption_time = SDL_GetTicks();

        cpu8080_generate_interruption(arcade->cpu, video_isr);

        video_isr = 3 - video_isr;
    }

    return 0;
}

int arcade_emulate_cpu(void* a)
{
    Arcade* arcade = (Arcade*) a;
    
    while (arcade->running)
    {
        cpu8080_emulate(arcade->cpu, NULL);
    }

    return 0;
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
    SDL_Delay(5e+2);
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
            arcade_handle_event(arcade, &event, &quit);
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