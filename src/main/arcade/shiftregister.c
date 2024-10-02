
#include "../../include/arcade/arcade.h"
#include "../../include/arcade/private/arcade.h"

#define SHIFT_DATA_PORT     OUT_PORT_4        // where the data to shift is output by the CPU --> emplace_high
#define SHIFT_OFFSET_PORT   OUT_PORT_2        // where offset is output by the CPU            --> set_offset
#define SHIFT_RESULT_PORT   IN_PORT_3         // where shift result is read by the CPU        --> get shift result


void shift_register_emplace_high(ShiftRegister* reg, uint8_t data)
{
    uint16_t v = data & 0x00ff;
    reg->_register = reg->_register >> 8;
    v = v << 8;
    reg->_register = reg->_register | v;
}

void shift_register_set_offset(ShiftRegister* reg, uint8_t offset_value)
{
    // Check offset data < 0b1000
    if (offset_value > 0b111)
        exit(1);

    reg->_offset = offset_value;
}

uint8_t shift_register_get(ShiftRegister* reg)
{
    uint16_t r = reg->_register;
    r = r >> reg->_offset;
    r |= 0x00ff;
    return (uint8_t) r;
}

uint8_t on_result_out(void* reg)
{
    return shift_register_get((ShiftRegister*) reg);
}

void on_data_in(void* reg, uint8_t data)
{
    shift_register_emplace_high((ShiftRegister*) reg, data);
}

void on_offset_in(void* reg, uint8_t offset)
{
    shift_register_emplace_high((ShiftRegister*) reg, offset);
}

void arcade_init_shift_register(Arcade* arcade)
{
    arcade->shifter = (ShiftRegister*) malloc(sizeof(ShiftRegister));

    cpu8080_register_output_callback(arcade->cpu, arcade->shifter, on_data_in, SHIFT_DATA_PORT);
    cpu8080_register_output_callback(arcade->cpu, arcade->shifter, on_offset_in, SHIFT_OFFSET_PORT);
    cpu8080_register_input_callback(arcade->cpu, arcade->shifter, on_result_out, SHIFT_RESULT_PORT);
}