#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include <stdint.h>
#include <stdlib.h>

typedef struct ShiftHardware
{
    uint16_t data;
    uint8_t offset;
} ShiftHardware;

void shift_register_emplace_high(ShiftHardware* h, uint8_t data);

void shift_register_set_offset(ShiftHardware* h, uint8_t offset);

uint8_t shift_register_get(ShiftHardware* h);

#endif //SHIFT_REGISTER_H