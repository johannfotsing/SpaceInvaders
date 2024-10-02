/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080/cpu_8080.h"
#include "../../include/cpu_8080/private/cpu_8080.h"

/** IO operations **/
/*******************/

void cpu8080_write_io(Cpu8080* cpu, int port_number, int pin_number, uint8_t value)
{
    if (value == 1)
    {
        cpu->in[port_number].data |= (1 << pin_number);
    }
    else if (value == 0)
    {
        uint8_t mask = (1 << pin_number); mask = ~mask;
        cpu->in[port_number].data &= mask;
    }
    else
    {
        fprintf(stderr, "Bit value is either 0 or 1.");
        exit(0);
    }
}

void cpu8080_read_io(Cpu8080* cpu, int port_number, int pin_number, uint8_t* byte)
{
    *byte = cpu->out[port_number].data & (1 << port_number);
}

void cpu8080_write_port(Cpu8080* cpu, int port_number, uint8_t value)
{
    cpu->in[port_number].data = value;
}

void cpu8080_read_port(Cpu8080* cpu, int port_number, uint8_t* byte)
{
    *byte = cpu->out[port_number].data;
}

void cpu8080_register_output_callback(Cpu8080* cpu, void* caller, output_callback_t callback_fn, uint8_t port_number)
{
    cpu->output_processors[port_number] = caller;
    cpu->output_callbacks[port_number] = callback_fn;
}

void cpu8080_register_input_callback(Cpu8080* cpu, void* caller, input_callback_t callback_fn, uint8_t port_number)
{
    cpu->input_processors[port_number] = caller;
    cpu->input_callbacks[port_number] = callback_fn;
}