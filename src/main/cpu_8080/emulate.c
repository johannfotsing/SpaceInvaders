/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080/cpu_8080.h"
#include "../../include/cpu_8080/private/cpu_8080.h"


/** Emulation **/
/***************/

/// Number of cycles for each instruction of the 8080 CPU
uint8_t cpu8080Cycles[] = 
{
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,
	
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,
	
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, 
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11, 
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11, 
};

void cpu8080_emulate_op(Cpu8080* cpu, const uint8_t* code, int* nb_cycles)
{
    const uint8_t* opcode;
    if (code == NULL)
        opcode = &cpu->memory[cpu->program_counter];
    else 
        opcode = code;
#ifdef PRINT_OPS
//    cpu8080_disassemble_op(cpu, opcode);
#endif
    switch (*opcode)
    {
    case 0x00 : cpu->program_counter += 1; break;						// NOP
    case 0x01 : cpu->b = opcode[2]; cpu->c = opcode[1]; cpu->program_counter += 3; break;		// LXI B,D16
    case 0x02 :						// STAX B
    {
        cpu8080_write_membyte(cpu, ((uint16_t)cpu->b) << 8 | ((uint16_t) cpu->c), cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x03 :								// INX B
    {
        uint16_t w = (uint16_t) cpu->b;
        w = (w << 8) | cpu->c; w++;
        cpu->c = w & 0xff;
        cpu->b = (w >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x04 : cpu->b++; cpu8080_update_flags(cpu, cpu->b); cpu->program_counter += 1; break;			// INR B
    case 0x05 : cpu->b--; cpu8080_update_flags(cpu, cpu->b); cpu->program_counter += 1; break;			// DCR B
    case 0x06 : cpu->b = opcode[1]; cpu->program_counter += 2; break;				// MVI B,D8
    case 0x07 :						// RLC
    {
        uint8_t prev_bit_7 = (cpu->a >> 7) & 0x01;
        cpu->a = cpu->a << 1;
        cpu->a = cpu->a | prev_bit_7;
        cpu->flags.c = prev_bit_7;
        cpu->program_counter += 1;
        break;
    }
    case 0x08 : cpu->program_counter += 1; break;						// NOP
    case 0x09 :								// DAD B
    {
        uint16_t h = (uint16_t) cpu->h;
        uint16_t l = (uint16_t) cpu->l;
        uint16_t hl = h << 8 | l;
        uint16_t rh = (uint16_t) cpu->b;
        uint16_t rl = (uint16_t) cpu->c;
        uint16_t rhl = rh << 8 | rl;
        uint32_t r = (uint32_t) hl + (uint32_t) rhl;
        cpu->flags.c = (r > 0xffff);
        cpu->l = (uint8_t) r & 0xff;
        cpu->h = (uint8_t) (r >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x0a :						// LDAX B
    {
        cpu8080_read_membyte(cpu, ((uint16_t)cpu->b) << 8 | ((uint16_t) cpu->c), &cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x0b :								// DCX B
    {
        uint16_t w = (uint16_t) cpu->b;
        w = (w << 8) | cpu->c; w--;
        cpu->c = w & 0xff;
        cpu->b = (w >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x0c : cpu->c++; cpu8080_update_flags(cpu, cpu->c); cpu->program_counter += 1; break;			// INR C
    case 0x0d : cpu->c--; cpu8080_update_flags(cpu, cpu->c); cpu->program_counter += 1; break;			// DCR C
    case 0x0e : cpu->c = opcode[1]; cpu->program_counter += 2; break;				// MVI C,D8
    case 0x0f :						// RRC
    {
        uint8_t prev_bit_0 = cpu->a & 0x01;
        cpu->a = cpu->a >> 1;
        cpu->a = cpu->a | (prev_bit_0 << 7);
        cpu->flags.c = prev_bit_0;
        cpu->program_counter += 1;
        break;
    }
    case 0x10 : cpu->program_counter += 1; break;						// NOP
    case 0x11 : cpu->d = opcode[2]; cpu->e = opcode[1]; cpu->program_counter += 3; break;		// LXI D,D16
    case 0x12 :						// STAX D
    {
        cpu8080_write_membyte(cpu, ((uint16_t)cpu->d) << 8 | ((uint16_t) cpu->e), cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x13 :								// INX D
    {
        uint16_t w = (uint16_t) cpu->d;
        w = (w << 8) | cpu->e; w++;
        cpu->e = w & 0xff;
        cpu->d = (w >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x14 : cpu->d++; cpu8080_update_flags(cpu, cpu->d); cpu->program_counter += 1; break;			// INR D
    case 0x15 : cpu->d--; cpu8080_update_flags(cpu, cpu->d); cpu->program_counter += 1; break;			// DCR D
    case 0x16 : cpu->d = opcode[1]; cpu->program_counter += 2; break;				// MVI D,D8
    case 0x17 :						// RAL
    {
        uint8_t prev_bit_7 = (cpu->a >> 7) & 0x01;
        cpu->a = cpu->a << 1;
        cpu->a = cpu->a | cpu->flags.c;
        cpu->flags.c = prev_bit_7;
        cpu->program_counter += 1;
        break;
    }
    case 0x18 : cpu->program_counter += 1; break;						// NOP
    case 0x19 :								// DAD D
    {
        uint16_t h = (uint16_t) cpu->h;
        uint16_t l = (uint16_t) cpu->l;
        uint16_t hl = h << 8 | l;
        uint16_t rh = (uint16_t) cpu->d;
        uint16_t rl = (uint16_t) cpu->e;
        uint16_t rhl = rh << 8 | rl;
        uint32_t r = (uint32_t) hl + (uint32_t) rhl;
        cpu->flags.c = (r > 0xffff);
        cpu->l = (uint8_t) r & 0xff;
        cpu->h = (uint8_t) (r >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x1a :						// LDAX D
    {
        cpu8080_read_membyte(cpu, ((uint16_t)cpu->d) << 8 | ((uint16_t) cpu->e), &cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x1b :								// DCX D
    {
        uint16_t w = (uint16_t) cpu->d;
        w = (w << 8) | cpu->e; w--;
        cpu->e = w & 0xff;
        cpu->d = (w >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x1c : cpu->e++; cpu8080_update_flags(cpu, cpu->e); cpu->program_counter += 1; break;			// INR E
    case 0x1d : cpu->e--; cpu8080_update_flags(cpu, cpu->e); cpu->program_counter += 1; break;			// DCR E
    case 0x1e : cpu->e = opcode[1]; cpu->program_counter += 2; break;				// MVI E,D8
    case 0x1f :						// RAR
    {
        uint8_t prev_bit_0 = cpu->a & 0x01;
        cpu->a = cpu->a >> 1;
        cpu->a = cpu->a | (cpu->flags.c << 7);
        cpu->flags.c = prev_bit_0;
        cpu->program_counter += 1;
        break;
    }
    case 0x20 : cpu->program_counter += 1; break;						// NOP
    case 0x21 : cpu->h = opcode[2]; cpu->l = opcode[1]; cpu->program_counter += 3; break;		// LXI H,D16
    case 0x22 :						// SHLD adr
    {
        uint16_t mem_offset = opcode[2];
        mem_offset = (mem_offset << 8) | opcode[1];
        cpu8080_write_membyte(cpu, mem_offset, cpu->l);
        cpu8080_write_membyte(cpu, mem_offset + 1, cpu->h);
        cpu->program_counter += 3;
        break;
    }
    case 0x23 :								// INX H
    {
        uint16_t w = (uint16_t) cpu->h;
        w = (w << 8) | cpu->l; w++;
        cpu->l = w & 0xff;
        cpu->h = (w >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x24 : cpu->h++; cpu8080_update_flags(cpu, cpu->h); cpu->program_counter += 1; break;			// INR H
    case 0x25 : cpu->h--; cpu8080_update_flags(cpu, cpu->h); cpu->program_counter += 1; break;			// DCR H
    case 0x26 : cpu->h = opcode[1]; cpu->program_counter += 2; break;				// MVI H,D8
    case 0x27 :						// DAA
    {
        cpu8080DAA(cpu);
        cpu->program_counter += 1;
        break;
    }
    case 0x28 : cpu->program_counter += 1; break;						// NOP
    case 0x29 :								// DAD H
    {
        uint16_t h = (uint16_t) cpu->h;
        uint16_t l = (uint16_t) cpu->l;
        uint16_t hl = h << 8 | l;
        uint16_t rh = (uint16_t) cpu->h;
        uint16_t rl = (uint16_t) cpu->l;
        uint16_t rhl = rh << 8 | rl;
        uint32_t r = (uint32_t) hl + (uint32_t) rhl;
        cpu->flags.c = (r > 0xffff);
        cpu->l = (uint8_t) r & 0xff;
        cpu->h = (uint8_t) (r >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x2a :						// LHLD adr
    {
        uint16_t mem_offset = opcode[2];
        mem_offset = (mem_offset << 8) | opcode[1];
        cpu8080_read_membyte(cpu, mem_offset, &cpu->l);
        cpu8080_read_membyte(cpu, mem_offset + 1, &cpu->h);
        cpu->program_counter += 3;
        break;
    }
    case 0x2b :								// DCX H
    {
        uint16_t w = (uint16_t) cpu->h;
        w = (w << 8) | cpu->l; w--;
        cpu->l = w & 0xff;
        cpu->h = (w >> 8) & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x2c : cpu->l++; cpu8080_update_flags(cpu, cpu->l); cpu->program_counter += 1; break;			// INR L
    case 0x2d : cpu->l--; cpu8080_update_flags(cpu, cpu->l); cpu->program_counter += 1; break;			// DCR L
    case 0x2e : cpu->l = opcode[1]; cpu->program_counter += 2; break;				// MVI L, D8
    case 0x2f : cpu->a = ~cpu->a; cpu->program_counter += 1; break;				// CMA
    case 0x30 : cpu->program_counter += 1; break;						// NOP
    case 0x31 :						// LXI SP, D16
    {
        uint16_t h = opcode[2]; uint16_t l = opcode[1];
        cpu->stack_pointer = h << 8 | l;
        cpu->program_counter += 3;
        break;
    }
    case 0x32 :						// STA adr
    {
        cpu8080_write_membyte(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]), cpu->a);
        cpu->program_counter += 3;
        break;
    }
    case 0x33 : cpu->stack_pointer++; cpu->program_counter += 1; break;				// INX SP
    case 0x34 :	                    // INR M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu8080_write_HL_membyte(cpu, hl_membyte + 1);
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu8080_update_flags(cpu, hl_membyte);
        cpu->program_counter += 1;
        break;
    }
    case 0x35 :	                    // DCR M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu8080_write_HL_membyte(cpu, hl_membyte - 1);
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu8080_update_flags(cpu, hl_membyte);
        cpu->program_counter += 1;
        break;
    }
    case 0x36 : cpu8080_write_HL_membyte(cpu, opcode[1]); cpu->program_counter += 2; break;		// MVI M,D8
    case 0x37 : cpu->flags.c = 0b1; cpu->program_counter += 1; break;				// STC
    case 0x38 : cpu->program_counter += 1; break;						// NOP
    case 0x39 :								// DAD SP
    {
        uint16_t h = (uint16_t) cpu->h;
        uint16_t l = (uint16_t) cpu->l;
        uint16_t hl = h << 8 | l;
        uint32_t r = (uint32_t) hl + (uint32_t) cpu->stack_pointer;
        cpu->flags.c = (r > 0xffff);
        cpu->l = (uint8_t) r & 0xff;
        cpu->h = (uint8_t) r >> 8 & 0xff;
        cpu->program_counter += 1;
        break;
    }
    case 0x3a :						// LDA adr
    {
        cpu8080_read_membyte(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]), &cpu->a);
        cpu->program_counter += 3;
        break;
    }
    case 0x3b : cpu->stack_pointer--; cpu->program_counter += 1; break;				// DCX SP
    case 0x3c : cpu->a++; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// INR A
    case 0x3d : cpu->a--; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// DCR A
    case 0x3e : cpu->a = opcode[1]; cpu->program_counter += 2; break;				// MVI A,D8
    case 0x3f : cpu->flags.c = ~cpu->flags.c; cpu->program_counter += 1; break;			// CMC
    case 0x40 : cpu->b = cpu->b; cpu->program_counter += 1; break;				// MOV B,B
    case 0x41 : cpu->b = cpu->c; cpu->program_counter += 1; break;				// MOV B,C
    case 0x42 : cpu->b = cpu->d; cpu->program_counter += 1; break;				// MOV B,D
    case 0x43 : cpu->b = cpu->e; cpu->program_counter += 1; break;				// MOV B,E
    case 0x44 : cpu->b = cpu->h; cpu->program_counter += 1; break;				// MOV B,H
    case 0x45 : cpu->b = cpu->l; cpu->program_counter += 1; break;				// MOV B,L
    case 0x46 : cpu8080_read_HL_membyte(cpu, &cpu->b); cpu->program_counter += 1; break;			// MOV B,M
    case 0x47 : cpu->b = cpu->a; cpu->program_counter += 1; break;				// MOV B,A
    case 0x48 : cpu->c = cpu->b; cpu->program_counter += 1; break;				// MOV C,B
    case 0x49 : cpu->c = cpu->c; cpu->program_counter += 1; break;				// MOV C,C
    case 0x4a : cpu->c = cpu->d; cpu->program_counter += 1; break;				// MOV C,D
    case 0x4b : cpu->c = cpu->e; cpu->program_counter += 1; break;				// MOV C,E
    case 0x4c : cpu->c = cpu->h; cpu->program_counter += 1; break;				// MOV C,H
    case 0x4d : cpu->c = cpu->l; cpu->program_counter += 1; break;				// MOV C,L
    case 0x4e : cpu8080_read_HL_membyte(cpu, &cpu->c); cpu->program_counter += 1; break;			// MOV C,M
    case 0x4f : cpu->c = cpu->a; cpu->program_counter += 1; break;				// MOV C,A
    case 0x50 : cpu->d = cpu->b; cpu->program_counter += 1; break;				// MOV D,B
    case 0x51 : cpu->d = cpu->c; cpu->program_counter += 1; break;				// MOV D,C
    case 0x52 : cpu->d = cpu->d; cpu->program_counter += 1; break;				// MOV D,D
    case 0x53 : cpu->d = cpu->e; cpu->program_counter += 1; break;				// MOV D,E
    case 0x54 : cpu->d = cpu->h; cpu->program_counter += 1; break;				// MOV D,H
    case 0x55 : cpu->d = cpu->l; cpu->program_counter += 1; break;				// MOV D,L
    case 0x56 : cpu8080_read_HL_membyte(cpu, &cpu->d); cpu->program_counter += 1; break;			// MOV D,M
    case 0x57 : cpu->d = cpu->a; cpu->program_counter += 1; break;				// MOV D,A
    case 0x58 : cpu->e = cpu->b; cpu->program_counter += 1; break;				// MOV E,B
    case 0x59 : cpu->e = cpu->c; cpu->program_counter += 1; break;				// MOV E,C
    case 0x5a : cpu->e = cpu->d; cpu->program_counter += 1; break;				// MOV E,D
    case 0x5b : cpu->e = cpu->e; cpu->program_counter += 1; break;				// MOV E,E
    case 0x5c : cpu->e = cpu->h; cpu->program_counter += 1; break;				// MOV E,H
    case 0x5d : cpu->e = cpu->l; cpu->program_counter += 1; break;				// MOV E,L
    case 0x5e : cpu8080_read_HL_membyte(cpu, &cpu->e); cpu->program_counter += 1; break;			// MOV E,M
    case 0x5f : cpu->e = cpu->a; cpu->program_counter += 1; break;				// MOV E,A
    case 0x60 : cpu->h = cpu->b; cpu->program_counter += 1; break;				// MOV H,B
    case 0x61 : cpu->h = cpu->c; cpu->program_counter += 1; break;				// MOV H,C
    case 0x62 : cpu->h = cpu->d; cpu->program_counter += 1; break;				// MOV H,D
    case 0x63 : cpu->h = cpu->e; cpu->program_counter += 1; break;				// MOV H,E
    case 0x64 : cpu->h = cpu->h; cpu->program_counter += 1; break;				// MOV H,H
    case 0x65 : cpu->h = cpu->l; cpu->program_counter += 1; break;				// MOV H,L
    case 0x66 : cpu8080_read_HL_membyte(cpu, &cpu->h); cpu->program_counter += 1; break;			// MOV H,M
    case 0x67 : cpu->h = cpu->a; cpu->program_counter += 1; break;				// MOV H,A
    case 0x68 : cpu->l = cpu->b; cpu->program_counter += 1; break;				// MOV L,B
    case 0x69 : cpu->l = cpu->c; cpu->program_counter += 1; break;				// MOV L,C
    case 0x6a : cpu->l = cpu->d; cpu->program_counter += 1; break;				// MOV L,D
    case 0x6b : cpu->l = cpu->e; cpu->program_counter += 1; break;				// MOV L,E
    case 0x6c : cpu->l = cpu->h; cpu->program_counter += 1; break;				// MOV L,H
    case 0x6d : cpu->l = cpu->l; cpu->program_counter += 1; break;				// MOV L,L
    case 0x6e : cpu8080_read_HL_membyte(cpu, &cpu->l); cpu->program_counter += 1; break;			// MOV L,M
    case 0x6f : cpu->l = cpu->a; cpu->program_counter += 1; break;				// MOV L,A
    case 0x70 : cpu8080_write_HL_membyte(cpu, cpu->b); cpu->program_counter += 1; break;			// MOV M,B
    case 0x71 : cpu8080_write_HL_membyte(cpu, cpu->c); cpu->program_counter += 1; break;			// MOV M,C
    case 0x72 : cpu8080_write_HL_membyte(cpu, cpu->d); cpu->program_counter += 1; break;			// MOV M,D
    case 0x73 : cpu8080_write_HL_membyte(cpu, cpu->e); cpu->program_counter += 1; break;			// MOV M,E
    case 0x74 : cpu8080_write_HL_membyte(cpu, cpu->h); cpu->program_counter += 1; break;			// MOV M,H
    case 0x75 : cpu8080_write_HL_membyte(cpu, cpu->l); cpu->program_counter += 1; break;			// MOV M,L
    case 0x76 : cpu->program_counter += 1; cpu->stopped = false; break;		    // HLT
    case 0x77 : cpu8080_write_HL_membyte(cpu, cpu->a); cpu->program_counter += 1; break;			// MOV M,A
    case 0x78 : cpu->a = cpu->b; cpu->program_counter += 1; break;				// MOV A,B
    case 0x79 : cpu->a = cpu->c; cpu->program_counter += 1; break;				// MOV A,C
    case 0x7a : cpu->a = cpu->d; cpu->program_counter += 1; break;				// MOV A,D
    case 0x7b : cpu->a = cpu->e; cpu->program_counter += 1; break;				// MOV A,E
    case 0x7c : cpu->a = cpu->h; cpu->program_counter += 1; break;				// MOV A,H
    case 0x7d : cpu->a = cpu->l; cpu->program_counter += 1; break;				// MOV A,L
    case 0x7e : cpu8080_read_HL_membyte(cpu, &cpu->a); cpu->program_counter += 1; break;			// MOV A,M
    case 0x7f : cpu->a = cpu->a; cpu->program_counter += 1; break;				// MOV A,A
    case 0x80 :									// ADD B
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->b);
        cpu->program_counter += 1;
        break;
    }
    case 0x81 :									// ADD C
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->c);
        cpu->program_counter += 1;
        break;
    }
    case 0x82 :									// ADD D
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->d);
        cpu->program_counter += 1;
        break;
    }
    case 0x83 :									// ADD E
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->e);
        cpu->program_counter += 1;
        break;
    }
    case 0x84 :									// ADD H
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->h);
        cpu->program_counter += 1;
        break;
    }
    case 0x85 :									// ADD L
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->l);
        cpu->program_counter += 1;
        break;
    }
    case 0x86 :									// ADD M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, hl_membyte);
        cpu->program_counter += 1;
        break;
    }
    case 0x87 :									// ADD A
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x88 :									// ADC B
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->b);
        cpu->program_counter += 1;
        break;
    }
    case 0x89 :									// ADC C
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->c);
        cpu->program_counter += 1;
        break;
    }
    case 0x8a :									// ADC D
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->d);
        cpu->program_counter += 1;
        break;
    }
    case 0x8b :									// ADC E
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->e);
        cpu->program_counter += 1;
        break;
    }
    case 0x8c :									// ADC H
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->h);
        cpu->program_counter += 1;
        break;
    }
    case 0x8d :									// ADC L
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->l);
        cpu->program_counter += 1;
        break;
    }
    case 0x8e :									// ADC M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, hl_membyte);
        cpu->program_counter += 1;
        break;
    }
    case 0x8f :									// ADC A
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x90 :									// SUB B
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->b);
        cpu->program_counter += 1;
        break;
    }
    case 0x91 :									// SUB C
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->c);
        cpu->program_counter += 1;
        break;
    }
    case 0x92 :									// SUB D
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->d);
        cpu->program_counter += 1;
        break;
    }
    case 0x93 :									// SUB E
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->e);
        cpu->program_counter += 1;
        break;
    }
    case 0x94 :									// SUB H
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->h);
        cpu->program_counter += 1;
        break;
    }
    case 0x95 :									// SUB L
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->l);
        cpu->program_counter += 1;
        break;
    }
    case 0x96 :									// SUB M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, hl_membyte);
        cpu->program_counter += 1;
        break;
    }
    case 0x97 :									// SUB A
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0x98 :									// SBB B
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->b);
        cpu->program_counter += 1;
        break;
    }
    case 0x99 :									// SBB C
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->c);
        cpu->program_counter += 1;
        break;
    }
    case 0x9a :									// SBB D
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->d);
        cpu->program_counter += 1;
        break;
    }
    case 0x9b :									// SBB E
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->e);
        cpu->program_counter += 1;
        break;
    }
    case 0x9c :									// SBB H
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->h);
        cpu->program_counter += 1;
        break;
    }
    case 0x9d :									// SBB L
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->l);
        cpu->program_counter += 1;
        break;
    }
    case 0x9e :									// SBB M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, hl_membyte);
        cpu->program_counter += 1;
        break;
    }
    case 0x9f :									// SBB A
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0xa0 : cpu->a &= cpu->b; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA B
    case 0xa1 : cpu->a &= cpu->c; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA C
    case 0xa2 : cpu->a &= cpu->d; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA D
    case 0xa3 : cpu->a &= cpu->e; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA E
    case 0xa4 : cpu->a &= cpu->h; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA H
    case 0xa5 : cpu->a &= cpu->l; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA L
    case 0xa6 :		                // ANA M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a &= hl_membyte;
        cpu8080_update_flags(cpu, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0xa7 : cpu->a &= cpu->a; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ANA A
    case 0xa8 : cpu->a ^= cpu->b; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA B
    case 0xa9 : cpu->a ^= cpu->c; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA C
    case 0xaa : cpu->a ^= cpu->d; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA D
    case 0xab : cpu->a ^= cpu->e; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA E
    case 0xac : cpu->a ^= cpu->h; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA H
    case 0xad : cpu->a ^= cpu->l; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA L
    case 0xae :		// XRA M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a ^= hl_membyte;
        cpu8080_update_flags(cpu, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0xaf : cpu->a ^= cpu->a; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// XRA A
    case 0xb0 : cpu->a |= cpu->b; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA B
    case 0xb1 : cpu->a |= cpu->c; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA C
    case 0xb2 : cpu->a |= cpu->d; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA D
    case 0xb3 : cpu->a |= cpu->e; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA E
    case 0xb4 : cpu->a |= cpu->h; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA H
    case 0xb5 : cpu->a |= cpu->l; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA L
    case 0xb6 :                 		// ORA M
    {
        uint8_t hl_membyte;
        cpu8080_read_HL_membyte(cpu, &hl_membyte);
        cpu->a |= hl_membyte;
        cpu8080_update_flags(cpu, cpu->a);
        cpu->program_counter += 1;
        break;
    }
    case 0xb7 : cpu->a |= cpu->a; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 1; break;			// ORA A
    case 0xb8 : cpu->flags.c = cpu->a < cpu->b; cpu->flags.z = cpu->a == cpu->b; cpu->program_counter += 1; break;		// CMP B
    case 0xb9 : cpu->flags.c = cpu->a < cpu->c; cpu->flags.z = cpu->a == cpu->c; cpu->program_counter += 1; break;		// CMP C
    case 0xba : cpu->flags.c = cpu->a < cpu->d; cpu->flags.z = cpu->a == cpu->d; cpu->program_counter += 1; break;		// CMP D
    case 0xbb : cpu->flags.c = cpu->a < cpu->e; cpu->flags.z = cpu->a == cpu->e; cpu->program_counter += 1; break;		// CMP E
    case 0xbc : cpu->flags.c = cpu->a < cpu->h; cpu->flags.z = cpu->a == cpu->h; cpu->program_counter += 1; break;		// CMP H
    case 0xbd : cpu->flags.c = cpu->a < cpu->l; cpu->flags.z = cpu->a == cpu->l; cpu->program_counter += 1; break;		// CMP L
    case 0xbe :				// CMP M
    {
        uint8_t hl_memory;
        cpu8080_read_HL_membyte(cpu, &hl_memory);
        cpu->flags.c = cpu->a < hl_memory;
        cpu->flags.z = cpu->a == hl_memory;
        cpu->program_counter += 1;
        break;
    }
    case 0xbf : cpu->flags.c = cpu->a < cpu->a; cpu->flags.z = cpu->a == cpu->a; cpu->program_counter += 1; break;		// CMP A
    case 0xc0 :						// RNZ
    {
        cpu->program_counter += 1;
        if (cpu->flags.z) break;
        uint8_t pcl, pch; 
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xc1 :						// POP B
    {
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->c);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->b);
        cpu->stack_pointer++;
        cpu->program_counter += 1;
        break;
    }
    case 0xc2 :						// JNZ adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.z) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xc3 :						// JMP adr
    {
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xc4 :						// CNZ adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.z) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xc5 :						// PUSH B
    {
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->b);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->c);
        cpu->program_counter += 1;
        break;
    }
    case 0xc6 :									// ADI D8
    {
        cpu->a = cpu8080_ALU_add(cpu, cpu->a, opcode[1]);
        cpu->program_counter += 2;
        break;
    }
    case 0xc7 :						// RST 0
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x0;
        break;
    }
    case 0xc8 :						// RZ
    {
        cpu->program_counter += 1;
        if (!cpu->flags.z) break;
        uint8_t pch, pcl;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xc9 :						// RET
    {
        uint8_t pch, pcl;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xca :						// JZ adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.z) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xcb : cpu->program_counter += 1; break;						// NOP
    case 0xcc :						// CZ adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.z) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xcd :						// CALL adr
    #ifdef FOR_CPUDIAG
        if (5 ==  ((opcode[2] << 8) | opcode[1]))
        {
            if (cpu->c == 9)
            {
                uint16_t offset = (cpu->d << 8) | (cpu->e);
                char *str = &cpu->memory[offset + 3];  //skip the prefix bytes
                while (*str != '$')
                    printf("%c", *str++);
                printf("\n");
            }
            else if (cpu->c == 2)
            {    
                //saw this in the inspected code, never saw it called
                printf ("print char routine called\n");
            }
            break;
        }
        else if (0 ==  ((opcode[2] << 8) | opcode[1]))
        {
            exit(0);
        }
        else
    #endif
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xce :									// ACI D8
    {
        cpu->a = cpu8080_ALU_add_with_carry(cpu, cpu->a, opcode[1]);
        cpu->program_counter += 2;
        break;
    }
    case 0xcf :						// RST 1
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x8;
        break;
    }
    case 0xd0 :						// RNC
    {
        cpu->program_counter += 1;
        if (cpu->flags.c) break;
        uint8_t pcl, pch;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xd1 :						// POP D
    {
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->e);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->d);
        cpu->stack_pointer++;
        cpu->program_counter += 1;
        break;
    }
    case 0xd2 :						// JNC adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.c) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xd3 :		                // OUT D8
    {
        uint8_t port_number = opcode[1];
        cpu->out[port_number].data = cpu->a;
        (*cpu->output_callbacks[port_number])(cpu->output_processors[port_number], cpu->out[port_number].data);
        cpu->program_counter += 2; 
        break;
    }
    case 0xd4 :						// CNC adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.c) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xd5 :						// PUSH D
    {
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->d);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->e);
        cpu->program_counter += 1;
        break;
    }
    case 0xd6 :									// SUI D8
    {
        cpu->a = cpu8080_ALU_substract(cpu, cpu->a, opcode[1]);
        cpu->program_counter += 2;
        break;
    }
    case 0xd7 :						// RST 2
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x10;
        break;
    }
    case 0xd8 :						// RC
    {
        cpu->program_counter += 1;
        if (!cpu->flags.c) break;
        uint8_t pcl, pch;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xd9 : cpu->program_counter += 1; break;						// NOP
    case 0xda :						// JC adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.c) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xdb : 		            // IN D8
    {
        uint8_t port_number = opcode[1];
        cpu->in[port_number].data = (*cpu->input_callbacks[port_number])(cpu->input_processors[port_number]);
        cpu->a = cpu->in[port_number].data;
        cpu->program_counter += 2;
        break;
    }
    case 0xdc :						// CC adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.c) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xdd : cpu->program_counter += 1; break;						// NOP
    case 0xde :									// SBI D8
    {
        cpu->a = cpu8080_ALU_substract_with_borrow(cpu, cpu->a, opcode[1]);
        cpu->program_counter += 2;
        break;
    }
    case 0xdf :						// RST 3
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x18;
        break;
    }
    case 0xe0 :						// RPO
    {
        cpu->program_counter += 1;
        if (cpu->flags.p) break;
        uint8_t pcl, pch;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xe1 :						// POP H
    {
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->l);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->h);
        cpu->stack_pointer++;
        cpu->program_counter += 1;
        break;
    }
    case 0xe2 :						// JPO adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.p) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xe3 :						// XTHL
    {
        uint8_t l = cpu->l, h = cpu->h;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->l);
        cpu8080_read_membyte(cpu, cpu->stack_pointer + 1, &cpu->h);
        cpu8080_write_membyte(cpu, cpu->stack_pointer, l);
        cpu8080_write_membyte(cpu, cpu->stack_pointer + 1, h);
        cpu->program_counter += 1;
        break;
    }
    case 0xe4 :						// CPO adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.p) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xe5 :						// PUSH H
    {
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->h);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->l);
        cpu->program_counter += 1;
        break;
    }
    case 0xe6 : cpu->a &= opcode[1]; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 2; break;			// ANI D8
    case 0xe7 :						// RST 4
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x20;
        break;
    }
    case 0xe8 :						// RPE
    {
        cpu->program_counter += 1;
        if (!cpu->flags.p) break;
        uint8_t pcl, pch;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xe9 :						// PCHL
    {
        cpu->program_counter = cpu->h;
        cpu->program_counter = (cpu->program_counter << 8) | cpu->l;
        break;
    }
    case 0xea :						// JPE adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.p) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xeb :						// XCHG
    {
        uint8_t l = cpu->l, h = cpu->h;
        cpu->l = cpu->e; cpu->e = l;
        cpu->h = cpu->d; cpu->d = h;
        cpu->program_counter += 1;
        break;
    }
    case 0xec :						// CPE adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.p) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xed : cpu->program_counter += 1; break;						// NOP
    case 0xee : cpu->a ^= opcode[1]; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 2; break;			// XRI D8
    case 0xef :						// RST 5
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x28;
        break;
    }
    case 0xf0 :						// RP
    {
        cpu->program_counter += 1;
        if (cpu->flags.s) break;
        uint8_t pcl, pch;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xf1 :						// POP PSW
    {
        cpu8080_read_membyte(cpu, cpu->stack_pointer, (uint8_t*) &cpu->flags);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &cpu->a);
        cpu->stack_pointer++;
        cpu->program_counter += 1;
        break;
    }
    case 0xf2 :						// JP adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.s) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xf3 : cpu->program_counter += 1; cpu->interrupt_enabled = false; break;				// DI

    case 0xf4 :						// CP adr
    {
        cpu->program_counter += 3;
        if (cpu->flags.s) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xf5 :						// PUSH PSW
    {
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->a);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, *((uint8_t*) &cpu->flags));
        cpu->program_counter += 1;
        break;
    }
    case 0xf6 : cpu->a |= opcode[1]; cpu8080_update_flags(cpu, cpu->a); cpu->program_counter += 2; break;			// ORI D8
    case 0xf7 :						// RST 6
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x30;
        break;
    }
    case 0xf8 :						// RM
    {
        cpu->program_counter += 1;
        if (!cpu->flags.s) break;
        uint8_t pcl, pch;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pcl);
        cpu->stack_pointer++;
        cpu8080_read_membyte(cpu, cpu->stack_pointer, &pch);
        cpu->stack_pointer++;
        cpu->program_counter = pch;
        cpu->program_counter = (cpu->program_counter << 8) | pcl;
        break;
    }
    case 0xf9 :						// SPHL
    {
        cpu->stack_pointer = cpu->h;
        cpu->stack_pointer = (cpu->stack_pointer << 8) | cpu->l;
        cpu->program_counter += 1;
        break;
    }
    case 0xfa :						// JM adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.s) break;
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xfb : cpu->program_counter += 1; cpu->interrupt_enabled = true; break;				// EI

    case 0xfc :						// CM adr
    {
        cpu->program_counter += 3;
        if (!cpu->flags.s) break;
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = opcode[2];
        cpu->program_counter = (cpu->program_counter << 8) | opcode[1];
        break;
    }
    case 0xfd : cpu->program_counter += 1; break;						// NOP
    case 0xfe : cpu->flags.c = cpu->a < opcode[1]; cpu->flags.z = cpu->a == opcode[1]; cpu->program_counter += 2; break;	// CPI D8
    case 0xff :						// RST 7
    {
        uint8_t pcl = cpu->program_counter & 0xff;
        uint8_t pch = (cpu->program_counter >> 8) & 0xff;
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pch);
        cpu->stack_pointer--;
        cpu8080_write_membyte(cpu, cpu->stack_pointer, pcl);
        cpu->program_counter = 0x38;
        break;
    }
    default:
        break;
    }
#ifdef PRINT_STATE
    cpu8080_print_state(cpu);
#endif
    if (nb_cycles != NULL)
        *nb_cycles = cpu8080Cycles[*opcode];
}

void cpu8080_emulate(Cpu8080* cpu, int* nb_cycles)
{
    pthread_mutex_lock(&cpu->emulation_mutex);

    // Emulate only if not halted
    if (cpu->stopped)
        goto unlock;

    cpu8080_emulate_op(cpu, NULL, nb_cycles);

unlock:
    pthread_mutex_unlock(&cpu->emulation_mutex);
}

void cpu8080_is_halted(Cpu8080* cpu, bool* halted)
{
    pthread_mutex_lock(&cpu->emulation_mutex);
    *halted = cpu->stopped;
    pthread_mutex_unlock(&cpu->emulation_mutex);
}

void cpu8080_is_interrupt_enabled(Cpu8080* cpu, bool* enabled)
{
    pthread_mutex_lock(&cpu->emulation_mutex);
    *enabled = cpu->interrupt_enabled;
    pthread_mutex_unlock(&cpu->emulation_mutex);
}

/** Display **/
/**************************/

void cpu8080_print_state(Cpu8080* cpu)
{
    /* print out processor state */    
    printf("\t8080 Flags:\t\tC=%d,P=%d,S=%d,Z=%d\n", 
        cpu->flags.c, cpu->flags.p, cpu->flags.s, cpu->flags.z);
    printf("\t8080 Registers:\t\tA=%02x B=%02x C=%02x D=%02x E=%02x H=%02x L=%02x SP=%04x\n",
        cpu->a, cpu->b, cpu->c, cpu->d,
        cpu->e, cpu->h, cpu->l, cpu->stack_pointer);
    printf("\tProgram Counter:\tpc=%04x\n\n", cpu->program_counter);
}

/** Run **/
/**************************/

void cpu8080_run(Cpu8080* cpu)
{
    int useless;
    while(true) cpu8080_emulate(cpu, &useless);
}