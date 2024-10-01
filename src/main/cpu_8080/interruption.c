/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080.h"
#include "../../include/private/cpu_8080.h"


void cpu8080_generate_interruption(Cpu8080* cpu, const uint8_t isr_index)
{
    pthread_mutex_lock(&cpu->emulation_mutex);

    // Interrupt should be enabled
    if (!cpu->interrupt_enabled)
        goto unlock;

    uint8_t interrupt_service_routine[] = {0xc7 + 8 * isr_index, 0x00, 0x00};
    int op_bytes = cpu8080_disassemble_op(cpu, interrupt_service_routine);
    if (op_bytes == 0)
        exit(1);

    // Push program counter
    cpu->stack_pointer--;
    cpu->memory[cpu->stack_pointer] = cpu->program_counter & 0xff00 >> 8;
    cpu->stack_pointer--;
    cpu->memory[cpu->stack_pointer] = cpu->program_counter & 0x00ff;
    //*

    cpu8080_emulate_op(cpu, interrupt_service_routine, NULL);

unlock:
    pthread_mutex_unlock(&cpu->emulation_mutex);
    
    // It is assumed that the interrupt service routine will re-enable interrupts at the end of its execution (TRUE from the programers manual)
    // It is also assumed that ISR will restore Program Counter (TRUE from the programmers manual, which recommends calling RETURN which pops the previous context from stack)
    // TODO: check if previous statements are TRUE.
}