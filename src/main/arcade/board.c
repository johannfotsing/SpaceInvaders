#include <unistd.h>
#include <string.h>
#include "../../include/arcade/arcade.h"
#include "../../include/arcade/private/arcade.h"


void on_coin_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 0, 1);
}

void on_coin_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 0, 0);
}


// START

void on_P1_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 2, 1);
}

void on_P1_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 2, 0);
}

void on_P2_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 1, 1);
}

void on_P2_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 1, 0);
}


// SHOOT

void on_P1_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 4, 1);
}

void on_P1_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 4, 0);
}

void on_P2_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, 4, 1);
}

void on_P2_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, 4, 0);
}


// Left

void on_P1_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 5, 1);
}

void on_P1_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 5, 0);
}

void on_P2_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, 5, 1);
}

void on_P2_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, 5, 0);
}


// Right


void on_P1_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 6, 1);
}

void on_P1_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_1, 6, 0);
}

void on_P2_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, 6, 1);
}

void on_P2_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, IN_PORT_2, 6, 0);
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