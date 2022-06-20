/**
 * @file SpaceInvaders.c
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
#include "SpaceInvadersArcade.h"

int main(int argc, char** argv)
{
    // Create window and texture
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* w = SDL_CreateWindow("Space Invaders", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* rd = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* tx = SDL_CreateTexture(rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, MONITOR_WIDTH, MONITOR_HEIGHT);
    // Init SpaceInvadersArcade
    SpaceInvadersArcade* arc = initArcade((ArcadeDisplay) {w, rd, tx}, "rom/");
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
                case SDL_SCANCODE_C: onCoinPressed(arc);
                // P1
                case SDL_SCANCODE_UP: onP1ShootPressed(arc); break;
                case SDL_SCANCODE_DOWN: onP1StartPressed(arc); break;
                case SDL_SCANCODE_LEFT: onP1LeftPressed(arc); break;
                case SDL_SCANCODE_RIGHT: onP1RightPressed(arc); break;
                // P2
                case SDL_SCANCODE_Z: onP2ShootPressed(arc); break;
                case SDL_SCANCODE_S: onP2StartPressed(arc); break;
                case SDL_SCANCODE_Q: onP2LeftPressed(arc); break;
                case SDL_SCANCODE_D: onP2RightPressed(arc); break;
                // QUIT
                case SDL_SCANCODE_ESCAPE: quit = SDL_TRUE; break;
                default: break;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_C: onCoinReleased(arc);
                // P1
                case SDL_SCANCODE_UP: onP1ShootReleased(arc); break;
                case SDL_SCANCODE_DOWN: onP1StartReleased(arc); break;
                case SDL_SCANCODE_LEFT: onP1LeftReleased(arc); break;
                case SDL_SCANCODE_RIGHT: onP1RightReleased(arc); break;
                // P2
                case SDL_SCANCODE_Z: onP2ShootReleased(arc); break;
                case SDL_SCANCODE_S: onP2StartReleased(arc); break;
                case SDL_SCANCODE_Q: onP2LeftReleased(arc); break;
                case SDL_SCANCODE_D: onP2RightReleased(arc); break;
                // QUIT
                case SDL_SCANCODE_ESCAPE: quit = SDL_TRUE; break;
                default: break;
                }
            }
        }
    }
    // Clean up before leaving the program.
}