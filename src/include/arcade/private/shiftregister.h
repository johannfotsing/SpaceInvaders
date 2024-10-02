#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct ShiftRegister
{
    uint16_t _register;
    uint8_t _offset;
} ShiftRegister;

void shift_register_emplace_high(ShiftRegister* reg, uint8_t data);

void shift_register_set_offset(ShiftRegister* reg, uint8_t offset_value);

uint8_t shift_register_get(ShiftRegister* reg);

#endif //SHIFT_REGISTER_H