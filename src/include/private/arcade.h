/**
 * @file arcade.h
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ARCADE_PRIVATE_H
#define ARCADE_PRIVATE_H

#include <pthread.h>
#include "cpu_8080.h"
#include "../cpu_8080.h"
#include "../arcade.h"

struct Arcade
{
    const char* name;
    Screen display;
    Cpu8080* cpu;
    pthread_t cpu_thread;
    pthread_t display_thread;
    pthread_t video_interrupt_thread;
    bool running;
};

void arcade_init_display(Arcade* arcade, int screen_width, int screen_height);

void arcade_init_cpu(Arcade* arcade, const char* rom_folder_path);

/// Callback for shift register emulation
void on_shift_data_available(Cpu8080* cpu);

void emulate_space_invaders_interruption(Cpu8080* cpu, uint8_t interrupt_number);

void start_video_interruption_thread(Arcade* a);

void arcade_show(Arcade* a);

void arcade_update_display(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format);
void arcade_update_display_test(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format);

#endif // ARCADE_PRIVATE_H