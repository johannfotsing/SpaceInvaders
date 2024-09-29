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

#ifndef SPACEINVADERS_ARCADE
#define SPACEINVADERS_ARCADE

#include <SDL2/SDL.h>

typedef struct Arcade Arcade;

typedef struct Screen
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_PixelFormat* px_format;
    int width;
    int height;
} Screen;

Arcade* arcade_init(SDL_Window* w, SDL_Renderer* rd, SDL_Texture* tx, const char* rom_folder);

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

#endif // SPACEINVADERS_ARCADE