/**
 * @file SpaceInvadersArcade.h
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

typedef struct s_SIArcade SpaceInvadersArcade;

typedef struct s_ArcadeDisplay
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
} ArcadeDisplay;

SpaceInvadersArcade* initArcade(ArcadeDisplay disp);
void freeArcade(SpaceInvadersArcade* a);

void onCoinPressed(SpaceInvadersArcade* a);
void onCoinReleased(SpaceInvadersArcade* a);
void onP1ShootPressed(SpaceInvadersArcade* a);
void onP1ShootReleased(SpaceInvadersArcade* a);
void onP1StartPressed(SpaceInvadersArcade* a);
void onP1StartReleased(SpaceInvadersArcade* a);
void onP1LeftPressed(SpaceInvadersArcade* a);
void onP1LeftReleased(SpaceInvadersArcade* a);
void onP1RightPressed(SpaceInvadersArcade* a);
void onP1RightReleased(SpaceInvadersArcade* a);
void onP2ShootPressed(SpaceInvadersArcade* a);
void onP2ShootReleased(SpaceInvadersArcade* a);
void onP2StartPressed(SpaceInvadersArcade* a);
void onP2StartReleased(SpaceInvadersArcade* a);
void onP2LeftPressed(SpaceInvadersArcade* a);
void onP2LeftReleased(SpaceInvadersArcade* a);
void onP2RightPressed(SpaceInvadersArcade* a);
void onP2RightReleased(SpaceInvadersArcade* a);

#endif // SPACEINVADERS_ARCADE