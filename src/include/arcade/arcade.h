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

#ifndef ARCADE_H
#define ARCADE_H

#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

/// Video
#define VIDEO_RAM_OFFSET 0x2400
#define WINDOW_WIDTH    672
#define WINDOW_HEIGHT   768

typedef struct Arcade Arcade;

typedef struct Screen
{
    int width;
    int height;
} Screen;

Arcade* arcade_init(const char* arcade_name, const char* rom_folder, int screen_width, int screen_height);

void arcade_free(Arcade* a);

void arcade_start(Arcade* a);

#endif // ARCADE_H