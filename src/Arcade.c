/**
 * @file Arcade.c
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

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

#include "Arcade_private.h"

void onCoinPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 0, 1);
}

void onCoinReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 0, 0);
}

void onP2StartPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 1, 1);
}

void onP2StartReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 1, 0);
}

void onP1StartPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 2, 1);
}

void onP1StartReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 2, 0);
}

void onP1ShootPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 4, 1);
}

void onP1ShootReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 4, 0);
}

void onP1LeftPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 5, 1);
}

void onP1LeftReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 5, 0);
}

void onP1RightPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 6, 1);
}

void onP1RightReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 6, 0);
}

void onP2ShootPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 4, 1);
}

void onP2ShootReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 4, 0);
}

void onP2LeftPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 5, 1);
}

void onP2LeftReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 5, 0);
}

void onP2RightPressed(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 6, 1);
}

void onP2RightReleased(Arcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 6, 0);
}