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

#ifndef BOARD_H
#define BOARD_H

#include "../../cpu_8080/cpu_8080.h"
#include "../arcade.h"

#define BIT_INSERT_COIN         0
#define BIT_P2_START            1
#define BIT_P1_START            2

#define BIT_P1_SHOOT            4 
#define BIT_P1_LEFT             5
#define BIT_P1_RIGHT            6

#define BIT_LIVES_1             0
#define BIT_LIVES_2             1
#define BIT_TILT                2
#define BIT_BONUS_LIFE          3
#define BIT_P2_SHOOT            4
#define BIT_P2_LEFT             5
#define BIT_P2_RIGHT            6
#define BIT_DISPLAY_COIN        7

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