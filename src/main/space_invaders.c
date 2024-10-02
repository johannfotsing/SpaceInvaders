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

#include <SDL2/SDL.h>
#include "../include/arcade/arcade.h"

#define SCREEN_WIDTH    224
#define SCREEN_HEIGHT   256

int main(int argc, char** argv)
{
    // Init Arcade
    Arcade* space_invaders = arcade_init("Space Invaders", "invaders", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Start SpaceInvaders Arcade
    arcade_start(space_invaders);

    // Clean up before leaving the program.
    arcade_free(space_invaders);
}