/**
 * @file joystick.h
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "../../cpu_8080/cpu_8080.h"
#include "../arcade.h"

void on_coin_pressed(Arcade* a);
void on_coin_released(Arcade* a);

void on_P1_shoot_pressed(Arcade* a);
void on_P1_shoot_released(Arcade* a);

void on_P1_start_pressed(Arcade* a);
void on_P1_start_released(Arcade* a);

void on_P1_left_pressed(Arcade* a);
void on_P1_left_released(Arcade* a);

void on_P1_right_pressed(Arcade* a);
void on_P1_right_released(Arcade* a);

void on_P2_shoot_pressed(Arcade* a);
void on_P2_shoot_released(Arcade* a);

void on_P2_start_pressed(Arcade* a);
void on_P2_start_released(Arcade* a);

void on_P2_left_pressed(Arcade* a);
void on_P2_start_released(Arcade* a);

void on_P2_right_pressed(Arcade* a);
void on_P2_right_released(Arcade* a);

#endif