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
#include "../../cpu_8080/cpu_8080.h"
#include "../../cpu_8080/private/cpu_8080.h"
#include "../arcade.h"
#include "shiftregister.h"
#include "board.h"

struct Arcade
{
    const char* name;
    Screen display;
    ShiftRegister* shifter;
    Cpu8080* cpu;

    bool running;
};

void arcade_init_cpu(Arcade* arcade, const char* rom_folder_path);
void arcade_init_shift_register(Arcade* arcade);
void arcade_init_display(Arcade* arcade, int screen_width, int screen_height);

int arcade_emulate_cpu(void* a);
int arcade_emulate_video_interruptions(void* a);
void arcade_update_display(Arcade* arcade, SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_PixelFormat* px_format);
void arcade_handle_joystick_event(Arcade* a, const SDL_Event* event, bool* quit);

#endif // ARCADE_PRIVATE_H