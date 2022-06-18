/**
 * @file Arcade.h
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef ARCADE
#define ARCADE

#include <SDL2/SDL.h>

typedef struct s_Arcade Arcade;

typedef struct s_ArcadeDisplay
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
} ArcadeDisplay;

Arcade* initArcade(ArcadeDisplay disp);

void onCoinPressed(Arcade* a);
void onCoinReleased(Arcade* a);
void onP1ShootPressed(Arcade* a);
void onP1ShootReleased(Arcade* a);
void onP1StartPressed(Arcade* a);
void onP1StartReleased(Arcade* a);
void onP1LeftPressed(Arcade* a);
void onP1LeftReleased(Arcade* a);
void onP1RightPressed(Arcade* a);
void onP1RightReleased(Arcade* a);
void onP2ShootPressed(Arcade* a);
void onP2ShootReleased(Arcade* a);
void onP2StartPressed(Arcade* a);
void onP2StartReleased(Arcade* a);
void onP2LeftPressed(Arcade* a);
void onP2LeftReleased(Arcade* a);
void onP2RightPressed(Arcade* a);
void onP2RightReleased(Arcade* a);

#endif // ARCADE