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
#include "../../include/arcade.h"
#include "../../include/private/arcade.h"

Arcade* arcade_init(const char* arcade_name, const char* rom_folder_path, int screen_width, int screen_height)
{
    Arcade* arcade = (Arcade*) malloc(sizeof(Arcade));

    // Store arcade name
    arcade->name = arcade_name;

    // Init CPU
    arcade_init_cpu(arcade, rom_folder_path);

    // Shift register init
    cpu8080_register_output_callback(arcade->cpu, &on_shift_data_available, SHFT_AMNT);

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
    
    const char* first_rom_name = "/invaders.h";
    size_t rom_filename_len = strlen(first_rom_name);
    size_t file_path_len = copy_len + rom_filename_len;
    char* rom_file_path = (char*) malloc(file_path_len + 1);
    rom_file_path = strcpy(rom_file_path, rom_folder_path);
    rom_file_path = strcat(rom_file_path, first_rom_name);
    
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0);
    rom_file_path[file_path_len-1] = 'g';
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x800);
    rom_file_path[file_path_len-1] = 'f';
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x1000);
    rom_file_path[file_path_len-1] = 'e';
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x1800);
    
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0);

    free(rom_file_path);
}

void arcade_free(Arcade* a)
{
    pthread_join(a->cpu_thread, NULL);
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

void arcade_screen_update_test(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format)
{
    int pitch;
    Uint32* texture_pixels;
    SDL_LockTexture(texture, NULL, (void**) &texture_pixels, &pitch);
    for (int byte_pixel_index = 0; byte_pixel_index < arcade->display.width * arcade->display.height / 8; ++byte_pixel_index)
    {
        // Read pixel values
        /*uint8_t image_byte;
        cpu8080_read_membyte(arcade->cpu, VIDEO_RAM_OFFSET + byte_pixel_index, &image_byte);*/

        // Update texture
        for (int pixel_index = byte_pixel_index * 8; pixel_index < (byte_pixel_index + 1) * 8; ++pixel_index)
        {
            // Turn bit pixel into color pixel
            /*int bit_idx = pixel_index % 8;
            uint8_t pix_val = (image_byte & (1 << (bit_idx + 1))) * 255;*/
            Uint8 pix_val = rand();
            texture_pixels[pixel_index] = SDL_MapRGB(px_format, pix_val, pix_val, pix_val);
        }
    }
    SDL_UnlockTexture(texture);

    // Update renderer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void arcade_screen_update(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format)
{
    int pitch;
    Uint32* texture_pixels;
    SDL_LockTexture(texture, NULL, (void**) &texture_pixels, &pitch);
    for (int byte_pixel_index = 0; byte_pixel_index < arcade->display.width * arcade->display.height / 8; ++byte_pixel_index)
    {
        // Read pixel values
        uint8_t image_byte;
        cpu8080_read_membyte(arcade->cpu, VIDEO_RAM_OFFSET + byte_pixel_index, &image_byte);

        // Update texture
        for (int pixel_index = byte_pixel_index * 8; pixel_index < (byte_pixel_index + 1) * 8; ++pixel_index)
        {
            // Turn bit pixel into color pixel
            int bit_idx = pixel_index % 8;
            uint8_t pix_val = (image_byte & (1 << (bit_idx + 1))) * 255;
            texture_pixels[pixel_index] = SDL_MapRGB(px_format, pix_val, pix_val, pix_val);
        }
    }
    SDL_UnlockTexture(texture);

    // Update renderer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void arcade_show(Arcade* arcade)
{
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
    SDL_PixelFormat* sdl_format = SDL_AllocFormat(px_format);

    // Event loop
    SDL_Event event;
    SDL_bool quit = SDL_FALSE;
    
    while (!quit)
    {
        arcade_screen_update(arcade, window, renderer, texture, sdl_format);

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;

            else if (event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_C: on_coin_pressed(arcade);
                // P1
                case SDL_SCANCODE_UP: on_P1_shoot_pressed(arcade); break;
                case SDL_SCANCODE_DOWN: on_P1_start_pressed(arcade); break;
                case SDL_SCANCODE_LEFT: on_P1_left_pressed(arcade); break;
                case SDL_SCANCODE_RIGHT: on_P1_right_pressed(arcade); break;
                // P2
                case SDL_SCANCODE_Z: on_P2_shoot_pressed(arcade); break;
                case SDL_SCANCODE_S: on_P2_start_pressed(arcade); break;
                case SDL_SCANCODE_Q: on_P2_left_pressed(arcade); break;
                case SDL_SCANCODE_D: on_P2_right_pressed(arcade); break;
                // QUIT
                case SDL_SCANCODE_ESCAPE: quit = SDL_TRUE; break;
                default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_C: on_coin_released(arcade);
                // P1
                case SDL_SCANCODE_UP: on_P1_shoot_released(arcade); break;
                case SDL_SCANCODE_DOWN: on_P1_start_released(arcade); break;
                case SDL_SCANCODE_LEFT: on_P1_left_released(arcade); break;
                case SDL_SCANCODE_RIGHT: on_P1_right_released(arcade); break;
                // P2
                case SDL_SCANCODE_Z: on_P2_shoot_released(arcade); break;
                case SDL_SCANCODE_S: on_P2_start_released(arcade); break;
                case SDL_SCANCODE_Q: on_P2_start_released(arcade); break;
                case SDL_SCANCODE_D: on_P2_right_released(arcade); break;
                // QUIT
                case SDL_SCANCODE_ESCAPE: quit = SDL_TRUE; break;
                default: break;
                }
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_FreeFormat(sdl_format);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void start_video_interruption_thread(Arcade* arcade)
{
    // __useconds_t half_display_period = 1e+6 / 120;
    uint8_t isr_idx = 2;
    while (1)
    {
        // usleep(half_display_period);
        cpu8080_generate_interruption(arcade->cpu, isr_idx);
        SDL_Delay(1e+3 / 120);
        isr_idx = 3 - isr_idx;
    }
}

void arcade_start(Arcade* arcade)
{
    pthread_create(&arcade->cpu_thread, NULL, cpu8080_run, arcade->cpu);
    pthread_create(&arcade->video_interrupt_thread, NULL, start_video_interruption_thread, arcade);
    pthread_create(&arcade->display_thread, NULL, arcade_show, arcade);
    pthread_join(arcade->display_thread, NULL);
}