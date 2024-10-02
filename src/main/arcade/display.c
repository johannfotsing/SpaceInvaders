#include "../../include/arcade/private/arcade.h"

void arcade_init_display(Arcade* arcade, int screen_width, int screen_height)
{
    arcade->display.width = screen_width;
    arcade->display.height = screen_height;
}

int arcade_emulate_video_interruptions(void* a)
{
    Arcade* arcade = (Arcade*) a;
    
    Uint32 video_interruption_period_ms = 1e+3 / (30 * 2);
    int sleep_time;
    Uint32 last_video_interruption_time = SDL_GetTicks();

    uint8_t video_isr = 1;
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

void arcade_update_display(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format)
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