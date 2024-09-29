/**
 * @file space_invaders.c
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief Here is implemented the main loop of the SpaceInvaders game emulation.
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#define WINDOW_WIDTH    672
#define WINDOW_HEIGHT   768
#define MONITOR_WIDTH   224
#define MONITOR_HEIGHT  256

#include <SDL2/SDL.h>
#include "../include/arcade.h"

int main(int argc, char** argv)
{
    // Create window and texture
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* w = SDL_CreateWindow("Space Invaders", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* rd = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* tx = SDL_CreateTexture(rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, MONITOR_WIDTH, MONITOR_HEIGHT);
    // Init Arcade
    Arcade* arc = arcade_init(w, rd, tx, "invaders");
    // Start SpaceInvaders Arcade
    arcade_start(arc);
    // Event loop
    SDL_Event event;
    SDL_bool quit = SDL_FALSE;
    while(!quit)
    {
        if(SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
                quit = SDL_TRUE;
            else if (event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_C: on_coin_pressed(arc);
                // P1
                case SDL_SCANCODE_UP: on_P1_shoot_pressed(arc); break;
                case SDL_SCANCODE_DOWN: on_P1_start_pressed(arc); break;
                case SDL_SCANCODE_LEFT: on_P1_left_pressed(arc); break;
                case SDL_SCANCODE_RIGHT: on_P1_right_pressed(arc); break;
                // P2
                case SDL_SCANCODE_Z: on_P2_shoot_pressed(arc); break;
                case SDL_SCANCODE_S: on_P2_start_pressed(arc); break;
                case SDL_SCANCODE_Q: on_P2_left_pressed(arc); break;
                case SDL_SCANCODE_D: on_P2_right_pressed(arc); break;
                // QUIT
                case SDL_SCANCODE_ESCAPE: quit = SDL_TRUE; break;
                default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_C: on_coin_released(arc);
                // P1
                case SDL_SCANCODE_UP: on_P1_shoot_released(arc); break;
                case SDL_SCANCODE_DOWN: on_P1_start_released(arc); break;
                case SDL_SCANCODE_LEFT: on_P1_left_released(arc); break;
                case SDL_SCANCODE_RIGHT: on_P1_right_released(arc); break;
                // P2
                case SDL_SCANCODE_Z: on_P2_shoot_released(arc); break;
                case SDL_SCANCODE_S: on_P2_start_released(arc); break;
                case SDL_SCANCODE_Q: on_P2_start_released(arc); break;
                case SDL_SCANCODE_D: on_P2_right_released(arc); break;
                // QUIT
                case SDL_SCANCODE_ESCAPE: quit = SDL_TRUE; break;
                default: break;
                }
            }
        }
    }
    // Clean up before leaving the program.
}