/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080/private/cpu_8080.h"

uint8_t cpu8080_ALU_add(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint16_t r16 = a; r16 += (uint16_t) b;
    cpu8080_update_flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b & 0x0f);
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = (r16 > 0xff);
    uint8_t r = r16 & 0xff;
    return r;
}

uint8_t cpu8080_ALU_add_with_carry(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint16_t r16 = a; r16 += (uint16_t) b; r16 += cpu->flags.c;
    cpu8080_update_flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b & 0x0f) + cpu->flags.c;
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = (r16 > 0xff);
    uint8_t r = r16 & 0xff;
    return r;
}

uint8_t cpu8080_ALU_twos_complement(uint8_t a)
{
    uint8_t r = ~a;
    r += 1;
    return r;
}

uint8_t cpu8080_ALU_substract(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint8_t b_cmp = cpu8080_ALU_twos_complement(b);
    uint16_t r16 = a; r16 += (uint16_t) b_cmp;
    cpu8080_update_flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b_cmp & 0x0f);
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = ((r16 >> 8) & 0x01) == 0;
    uint8_t r = r16 & 0xff;
    return r;
}

uint8_t cpu8080_ALU_substract_with_borrow(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint8_t b_cmp = cpu8080_ALU_twos_complement(b);
    uint8_t cy_cmp = cpu8080_ALU_twos_complement(cpu->flags.c);
    uint16_t r16 = a; r16 += (uint16_t) b_cmp; r16 += (uint16_t) cy_cmp;
    cpu8080_update_flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b_cmp & 0x0f) + (cy_cmp & 0x0f);
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = ((r16 >> 8) & 0x01) == 0;
    uint8_t r = r16 & 0xff;
    return r;
}

void cpu8080DAA(Cpu8080* cpu)
{
    if ((cpu->a & 0x0f) > 9 || cpu->flags.ac == 1)
    {
        cpu->a += 6;
        if ((cpu->a & 0x0f) + 6 > 0x0f) cpu->flags.ac = 1;
        else cpu->flags.ac = 0;
    }
    uint8_t ah = (cpu->a >> 4) & 0x0f;
    if (ah > 9 || cpu->flags.c == 1)
    {
        ah += 6;
        if (ah > 0x0f) cpu->flags.c = 1;
    }
    cpu->a = (cpu->a & 0x0f) | (ah << 4);
}

/** Helper functions for arithmetic operations **/
/************************************************/

void cpu8080_update_flags(Cpu8080* cpu, uint16_t r)
{
    // Sign
    cpu->flags.s = (r >> 7) & 0x01;
    // Zero
    cpu->flags.z = ((r & 0xff) == 0);
    // Parity
    int num_ones = 0;
    for (int i=0; i<8; ++i)
    {
        num_ones += (r & 0x01);
        r = r >> 1;
    }
    cpu->flags.p = 1 - (num_ones % 2);
}
