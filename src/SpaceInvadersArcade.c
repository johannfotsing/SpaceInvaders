/**
 * @file SpaceInvadersArcade.c
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

SpaceInvadersArcade* initArcade(ArcadeDisplay disp)
{
    SpaceInvadersArcade* arc = (SpaceInvadersArcade*) malloc(sizeof(SpaceInvadersArcade));
    arc->display = disp;
    Cpu8080Config cpuCfg = {4, 8, 64, 2.e+6};
    arc->cpu = initCpu8080(&cpuCfg);
}

void freeArcade(SpaceInvadersArcade* a)
{
    freeCpu8080(a->cpu);
    free(a);
}

void onCoinPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 0, 1);
}

void onCoinReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 0, 0);
}

void onP2StartPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 1, 1);
}

void onP2StartReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 1, 0);
}

void onP1StartPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 2, 1);
}

void onP1StartReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 2, 0);
}

void onP1ShootPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 4, 1);
}

void onP1ShootReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 4, 0);
}

void onP1LeftPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 5, 1);
}

void onP1LeftReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 5, 0);
}

void onP1RightPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 6, 1);
}

void onP1RightReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP1, 6, 0);
}

void onP2ShootPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 4, 1);
}

void onP2ShootReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 4, 0);
}

void onP2LeftPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 5, 1);
}

void onP2LeftReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 5, 0);
}

void onP2RightPressed(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 6, 1);
}

void onP2RightReleased(SpaceInvadersArcade* a)
{
    writeCpu8080IO(a->cpu, INP2, 6, 0);
}

void onShiftDataAvailable(Cpu8080* cpu)
{
    static uint16_t shift_register = 0x0000;
    uint16_t shift_data = readCpu8080Port(cpu, SHFT_DATA);
    shift_register = ((shift_register >> 8) & 0x00ff) | ((shift_data << 8) & 0xff00);
    uint8_t shift_offset = readCpu8080Port(cpu, SHFT_AMNT) & 0b111; // offset is 3 bits long max
    uint8_t shift_result = ((shift_register << shift_offset) >> 8) & 0xff;
    writeCpu8080Port(cpu, SHFT_IN, shift_result);
}