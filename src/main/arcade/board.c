#include <unistd.h>
#include <string.h>
#include "../../include/arcade/private/arcade.h"

#define PRESSED true
#define RELEASED false


void on_coin_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_INSERT_COIN, PRESSED);
}

void on_coin_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_INSERT_COIN, RELEASED);
}


// START

void on_P1_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_START, PRESSED);
}

void on_P1_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_START, RELEASED);
}

void on_P2_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P2_START, PRESSED);
}

void on_P2_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P2_START, RELEASED);
}


// SHOOT

void on_P1_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_SHOOT, PRESSED);
}

void on_P1_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_SHOOT, RELEASED);
}

void on_P2_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, BIT_P2_SHOOT, PRESSED);
}

void on_P2_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, BIT_P2_SHOOT, RELEASED);
}


// Left

void on_P1_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_LEFT, PRESSED);
}

void on_P1_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_LEFT, RELEASED);
}

void on_P2_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, BIT_P2_LEFT, PRESSED);
}

void on_P2_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, BIT_P2_LEFT, RELEASED);
}


// Right


void on_P1_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_RIGHT, PRESSED);
}

void on_P1_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, BIT_P1_RIGHT, RELEASED);
}

void on_P2_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, BIT_P2_RIGHT, PRESSED);
}

void on_P2_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, BIT_P2_RIGHT, RELEASED);
}

void arcade_handle_joystick_event(Arcade* a, const SDL_Event* event, bool* quit)
{
    if (event->type == SDL_QUIT)
        *quit = true;

    else if (event->type == SDL_KEYDOWN)
    {
        switch(event->key.keysym.scancode)
        {
        case SDL_SCANCODE_C: on_coin_pressed(a); break;
        // P1
        case SDL_SCANCODE_UP: on_P1_shoot_pressed(a); break;
        case SDL_SCANCODE_DOWN: on_P1_start_pressed(a); break;
        case SDL_SCANCODE_LEFT: on_P1_left_pressed(a); break;
        case SDL_SCANCODE_RIGHT: on_P1_right_pressed(a); break;
        // P2
        case SDL_SCANCODE_W: on_P2_shoot_pressed(a); break;
        case SDL_SCANCODE_S: on_P2_start_pressed(a); break;
        case SDL_SCANCODE_A: on_P2_left_pressed(a); break;
        case SDL_SCANCODE_D: on_P2_right_pressed(a); break;
        // QUIT
        case SDL_SCANCODE_ESCAPE: *quit = true; break;
        default: break;
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        switch(event->key.keysym.scancode)
        {
        case SDL_SCANCODE_C: on_coin_released(a); break;
        // P1
        case SDL_SCANCODE_UP: on_P1_shoot_released(a); break;
        case SDL_SCANCODE_DOWN: on_P1_start_released(a); break;
        case SDL_SCANCODE_LEFT: on_P1_left_released(a); break;
        case SDL_SCANCODE_RIGHT: on_P1_right_released(a); break;
        // P2
        case SDL_SCANCODE_W: on_P2_shoot_released(a); break;
        case SDL_SCANCODE_S: on_P2_start_released(a); break;
        case SDL_SCANCODE_A: on_P2_start_released(a); break;
        case SDL_SCANCODE_D: on_P2_right_released(a); break;
        // QUIT
        case SDL_SCANCODE_ESCAPE: *quit = true; break;
        default: break;
        }
    }
}