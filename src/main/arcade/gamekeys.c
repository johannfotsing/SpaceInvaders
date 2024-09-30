
#include <unistd.h>
#include <string.h>
#include "../../include/arcade.h"
#include "../../include/private/arcade.h"

void on_coin_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 0, 1);
}

void on_coin_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 0, 0);
}

void on_P2_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 1, 1);
}

void on_P2_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 1, 0);
}

void on_P1_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 2, 1);
}

void on_P1_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 2, 0);
}

void on_P1_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 4, 1);
}

void on_P1_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 4, 0);
}

void on_P1_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 5, 1);
}

void on_P1_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 5, 0);
}

void on_P1_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 6, 1);
}

void on_P1_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 6, 0);
}

void on_P2_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 4, 1);
}

void on_P2_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 4, 0);
}

void on_P2_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 5, 1);
}

void on_P2_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 5, 0);
}

void on_P2_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 6, 1);
}

void on_P2_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 6, 0);
}