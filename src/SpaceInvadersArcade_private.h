/**
 * @file Arcade_private.h
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SPACEINVADERS_ARCADE_PRIVATE
#define SPACEINVADERS_ARCADE_PRIVATE

#include "SpaceInvadersArcade.h"
#include "Cpu8080.h"
#include <SDL2/SDL.h>

struct s_SIArcade
{
    ArcadeDisplay display;
    Cpu8080* cpu;
};

/// Callback for shift register emulation
void onShiftDataAvailable(Cpu8080* cpu);

#endif // SPACEINVADERS_ARCADE_PRIVATE