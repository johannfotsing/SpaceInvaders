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

/// Map CPU 8080 input ports
#define INP0        0
#define INP1        1
#define INP2        2
#define SHFT_IN     3
/// Map CPU 8080 output ports
#define SHFT_AMNT   2
#define SOUND1      3
#define SHFT_DATA   4
#define SOUND2      5
#define WATCHDOG    6
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

void on_coin_pressed(Arcade* a);
void on_coin_released(Arcade* a);

void on_P1_shoot_pressed(Arcade* a);
void on_P1_shoot_released(Arcade* a);

void on_P1_start_pressed(Arcade* a);
void on_P1_start_released(Arcade* a);

void on_P1_left_pressed(Arcade* a);
void on_P1_left_released(Arcade* a);

void on_P1_right_pressed(Arcade* a);
void on_P1_right_released(Arcade* a);

void on_P2_shoot_pressed(Arcade* a);
void on_P2_shoot_released(Arcade* a);

void on_P2_start_pressed(Arcade* a);
void on_P2_start_released(Arcade* a);

void on_P2_left_pressed(Arcade* a);
void on_P2_start_released(Arcade* a);

void on_P2_right_pressed(Arcade* a);
void on_P2_right_released(Arcade* a);

#endif // ARCADE_H