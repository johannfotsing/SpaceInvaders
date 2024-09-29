/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080.h"
#include "../../include/private/cpu_8080.h"

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

uint8_t cpu8080_read_io(Cpu8080* cpu, int port_number, int pin_number)
{

}

void cpu8080_write_port(Cpu8080* cpu, int port_number, uint8_t value)
{
    cpu->in[port_number].data = value;
}

uint8_t cpu8080_read_port(Cpu8080* cpu, int port_number)
{
    return cpu->out[port_number].data;
}

void cpu8080_register_output_callback(Cpu8080* cpu, output_callback_t cb, uint8_t port_number)
{
    cpu->io_callbacks[port_number] = cb;
}

void cpu8080_generate_interruption(Cpu8080* cpu, const uint8_t* interrupt_opcode)
{
    if (!cpu->interrupt_enabled) return;
    int op_bytes = cpu8080_disassemble_op(cpu, interrupt_opcode);
    if (op_bytes == 0) exit(1);
    cpu->interrupt_enabled = 0;
    pthread_mutex_lock(&cpu->emulation_mutex);
    cpu8080_emulate_op(cpu, interrupt_opcode);
    pthread_mutex_unlock(&cpu->emulation_mutex);
    // It is assumed that the interrupt service routine will re-enable interrupts at the end of its execution
    // It is also assumed that ISR will restore Program Counter
    // TODO: check if previous statements are TRUE.
}