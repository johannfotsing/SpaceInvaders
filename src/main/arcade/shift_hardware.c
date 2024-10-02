
#include "../../include/arcade/arcade.h"
#include "../../include/arcade/private/arcade.h"

#define SHIFT_DATA_PORT     OUT_PORT_4        // where the data to shift is output by the CPU --> emplace_high
#define SHIFT_OFFSET_PORT   OUT_PORT_2        // where offset is output by the CPU            --> set_offset
#define SHIFT_RESULT_PORT   IN_PORT_3         // where shift result is read by the CPU        --> get shift result


void shift_register_emplace_high(ShiftHardware* hardware, uint8_t data)
{
    uint16_t v = data & 0x00ff;
    hardware->data = hardware->data >> 8;
    v = v << 8;
    hardware->data = hardware->data | v;
}

void shift_register_set_offset(ShiftHardware* hardware, uint8_t offset_value)
{
    // Check offset data < 0b1000
    if (offset_value > 0b111)
        exit(1);

    hardware->offset = offset_value;
}

uint8_t shift_register_get(ShiftHardware* hardware)
{
    uint16_t r = hardware->data;
    r = r >> hardware->offset;
    r |= 0x00ff;
    return (uint8_t) r;
}

uint8_t on_result_out(void* hardware)
{
    return shift_register_get((ShiftHardware*) hardware);
}

void on_data_in(void* hardware, uint8_t data)
{
    shift_register_emplace_high((ShiftHardware*) hardware, data);
}

void on_offset_in(void* hardware, uint8_t offset)
{
    shift_register_emplace_high((ShiftHardware*) hardware, offset);
}

void arcade_init_shift_register(Arcade* arcade)
{
    arcade->shifter = (ShiftHardware*) malloc(sizeof(ShiftHardware));

    cpu8080_register_output_callback(arcade->cpu, arcade->shifter, on_data_in, SHIFT_DATA_PORT);
    cpu8080_register_output_callback(arcade->cpu, arcade->shifter, on_offset_in, SHIFT_OFFSET_PORT);
    cpu8080_register_input_callback(arcade->cpu, arcade->shifter, on_result_out, SHIFT_RESULT_PORT);
}