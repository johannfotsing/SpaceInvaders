/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Cpu8080.h"
#include "Cpu8080_private.h"

/** Constructor & destructor **/
/******************************/

Cpu8080* initCpu8080(const Cpu8080Config* conf)
{
    // Allocate CPU memory space
    Cpu8080* c = (Cpu8080*) malloc(sizeof(Cpu8080));
    c->nsec_per_cycle = 1.e+9/conf->frequency;
    // Allocate memory
    size_t mem_bytes = conf->memory_size_kb*1024*sizeof(uint8_t);
    c->memory = (uint8_t*) malloc(mem_bytes);
    // Allocate IO ports
    c->in = (IOPort*) malloc(conf->in_ports*sizeof(IOPort));
    c->out = (IOPort*) malloc(conf->out_ports*sizeof(IOPort));
    // Allocate callbacks array
    c->io_callbacks = (output_callback_t*) malloc(conf->out_ports*sizeof(output_callback_t));
    // Enable interrupts
    c->interrupt_enabled = 1;
}

void freeCpu8080(Cpu8080* cpu)
{
    free(cpu->io_callbacks);
    free(cpu->in);
    free(cpu->out);
    free(cpu->memory);
}

/** Memory operations **/
/***********************/

void loadCpu8080ROM(Cpu8080* cpu, const char* rom_path, uint16_t memory_offset)
{
    // Open ROM file
    FILE* rom_file = fopen(rom_path, "rb");
    if (rom_file == NULL)
    {
        fprintf(stderr, "Failed to open file %s.", rom_path);
        exit(1);
    }
    // Find ROM size
    fseek(rom_file, 0L, SEEK_END);
    int rom_size = ftell(rom_file);
    fseek(rom_file, 0L, SEEK_SET);
    // Copy file stream into CPU memory
    fread(&cpu->memory[memory_offset], 1, rom_size, rom_file);
    // Close file
    if (fclose(rom_file) == EOF) exit(1);
}

void writeCpu8080Memory(Cpu8080* cpu, uint16_t adr, uint8_t value)
{
    cpu->memory[adr] = value;
}

uint8_t readCpu8080Memory(Cpu8080* cpu, uint16_t adr)
{
    return cpu->memory[adr];
}

void writeCpu8080MemoryAtHL(Cpu8080* cpu, uint8_t value)
{
    uint16_t adr = cpu->h;
    adr = adr << 8 | cpu->l;
    writeCpu8080Memory(cpu, adr, value);
}

uint8_t readCpu8080MemoryAtHL(Cpu8080* cpu)
{
    uint16_t adr = cpu->h;
    adr = adr << 8 | cpu->l;
    return readCpu8080Memory(cpu, adr);
}

/** IO operations **/
/*******************/

void writeCpu8080IO(Cpu8080* cpu, int port_number, int pin_number, uint8_t value)
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

uint8_t readCpu8080IO(Cpu8080* cpu, int port_number, int pin_number)
{

}

void writeCpu8080Port(Cpu8080* cpu, int port_number, uint8_t value)
{
    cpu->in[port_number].data = value;
}

uint8_t readCpu8080Port(Cpu8080* cpu, int port_number)
{
    return cpu->out[port_number].data;
}

void registerCpu8080OutputCallback(Cpu8080* cpu, output_callback_t cb, uint8_t port_number)
{
    cpu->io_callbacks[port_number] = cb;
}

/** Helper functions for arithmetic operations **/
/************************************************/

void updateCpu8080Flags(Cpu8080* cpu, uint16_t r)
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

uint8_t cpu8080Add(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint16_t r16 = a; r16 += (uint16_t) b;
    updateCpu8080Flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b & 0x0f);
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = (r16 > 0xff);
    uint8_t r = r16 & 0xff;
    return r;
}

uint8_t cpu8080AddWithCarry(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint16_t r16 = a; r16 += (uint16_t) b; r16 += cpu->flags.c;
    updateCpu8080Flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b & 0x0f) + cpu->flags.c;
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = (r16 > 0xff);
    uint8_t r = r16 & 0xff;
    return r;
}

uint8_t twoComplement(uint8_t a)
{
    uint8_t r = ~a;
    r += 1;
    return r;
}

uint8_t cpu8080Sub(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint8_t b_cmp = twoComplement(b);
    uint16_t r16 = a; r16 += (uint16_t) b_cmp;
    updateCpu8080Flags(cpu, r16);
    // Auxilliary carry
    uint8_t sum_of_nibbles = (a & 0x0f) + (b_cmp & 0x0f);
    uint8_t r16_nibble = r16 & 0x0f;
    cpu->flags.ac = r16_nibble < sum_of_nibbles;
    // Carry
    cpu->flags.c = ((r16 >> 8) & 0x01) == 0;
    uint8_t r = r16 & 0xff;
    return r;
}

uint8_t cpu8080SubWithBorrow(Cpu8080* cpu, uint8_t a, uint8_t b)
{
    uint8_t b_cmp = twoComplement(b);
    uint8_t cy_cmp = twoComplement(cpu->flags.c);
    uint16_t r16 = a; r16 += (uint16_t) b_cmp; r16 += (uint16_t) cy_cmp;
    updateCpu8080Flags(cpu, r16);
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

/** Other help functions **/
/**************************/

void printCpu8080State(Cpu8080* cpu)
{
    /* print out processor state */    
    printf("\t8080 Flags:\t\tC=%d,P=%d,S=%d,Z=%d\n", 
        cpu->flags.c, cpu->flags.p, cpu->flags.s, cpu->flags.z);
    printf("\t8080 Registers:\t\tA=%02x B=%02x C=%02x D=%02x E=%02x H=%02x L=%02x SP=%04x\n",
        cpu->a, cpu->b, cpu->c, cpu->d,
        cpu->e, cpu->h, cpu->l, cpu->sp);
    printf("\tProgram Counter:\tpc=%04x\n\n", cpu->pc);
}


/** Emulation **/
/***************/

int disassembleCpu8080Op(Cpu8080* cpu, const uint8_t* code)
{
    const uint8_t* opcode;
    if (code) opcode = code;
    else opcode = &cpu->memory[cpu->pc];
    int opbytes = 1;
    printf("%04x\t", cpu->pc);
    switch(*opcode)
    {
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI      B, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;
        case 0x02: printf("STAX     B"); break;
        case 0x03: printf("INX      B"); break;
        case 0x04: printf("INR      B"); break;
        case 0x05: printf("DCR      B"); break;
        case 0x06: printf("MVI      B, #$%02x", opcode[1]); opbytes=2; break;
        case 0x07: printf("RLC      B"); break;
        case 0x08: printf("NOP"); break;
        case 0x09: printf("DAD      B"); break;
        case 0x0a: printf("LDAX     B"); break;
        case 0x0b: printf("DCX      B"); break;
        case 0x0c: printf("INR      C"); break;                                                   //     1     Z, S, P, AC     C <- C+1
        case 0x0d: printf("DCR      C"); break;                                                   //     1     Z, S, P, AC     C <-C-1
        case 0x0e: printf("MVI      C, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          C <- byte 2
        case 0x0f: printf("RRC"); break;                                                          //     1     CY     A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
        case 0x10: printf("NOP"); break;               
        case 0x11: printf("LXI      D, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;          // D16 3          D <- byte 3, E <- byte 2
        case 0x12: printf("STAX     D"); break;                                                   //     1          (DE) <- A
        case 0x13: printf("INX      D"); break;                                                   //     1          DE <- DE + 1
        case 0x14: printf("INR      D"); break;                                                   //     1     Z, S, P, AC     D <- D+1
        case 0x15: printf("DCR      D"); break;                                                   //     1     Z, S, P, AC     D <- D-1
        case 0x16: printf("MVI      D, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          D <- byte 2
        case 0x17: printf("RAL"); break;                                                          //     1     CY     A = A << 1; bit 0 = prev CY; CY = prev bit 7
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD      D"); break;                                                   //     1     CY     HL = HL + DE
        case 0x1a: printf("LDAX     D"); break;                                                   //     1          A <- (DE)
        case 0x1b: printf("DCX      D"); break;                                                   //     1          DE = DE-1
        case 0x1c: printf("INR      E"); break;                                                   //     1     Z, S, P, AC     E <-E+1
        case 0x1d: printf("DCR      E"); break;                                                   //     1     Z, S, P, AC     E <- E-1
        case 0x1e: printf("MVI      E, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          E <- byte 2
        case 0x1f: printf("RAR"); break;                                                          //     1     CY     A = A >> 1; bit 7 = prev bit 7; CY = prev bit 0
        case 0x20: printf("RIM"); break;                                                          //     1          special
        case 0x21: printf("LXI      H, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;          //D16  3          H <- byte 3, L <- byte 2
        case 0x22: printf("SHLD     #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          (adr) <-L; (adr+1)<-H TODO: check
        case 0x23: printf("INX      H"); break;                                                   //     1          HL <- HL + 1
        case 0x24: printf("INR      H"); break;                                                   //     1     Z, S, P, AC     H <- H+1
        case 0x25: printf("DCR      H"); break;                                                   //     1     Z, S, P, AC     H <- H-1
        case 0x26: printf("MVI      H, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          L <- byte 2
        case 0x27: printf("DAA"); break;                                                          //     1          special
        case 0x28: printf("NOP"); break;
        case 0x29: printf("DAD      H"); break;                                                   //     1     CY     HL = HL + HI
        case 0x2a: printf("LHLD     #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          L <- (adr); H<-(adr+1)
        case 0x2b: printf("DCX      H"); break;                                                   //     1          HL = HL-1
        case 0x2c: printf("INR      L"); break;                                                   //     1     Z, S, P, AC     L <- L+1
        case 0x2d: printf("DCR      L"); break;                                                   //     1     Z, S, P, AC     L <- L-1
        case 0x2e: printf("MVI      L, #$%02x", opcode[1]); opbytes=2; break;                       //D8   2          L <- byte 2
        case 0x2f: printf("CMA"); break;                                                          //     1          A <- !A
        case 0x30: printf("SIM"); break;                                                          //     1          special
        case 0x31: printf("LXI      SP, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;         //D16  3          SP.hi <- byte 3, SP.lo <- byte 2
        case 0x32: printf("STA      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          (adr) <- A
        case 0x33: printf("INX      SP"); break;                                                  //     1          SP = SP + 1
        case 0x34: printf("INR      M"); break;                                                   //     1     Z, S, P, AC     (HL) <- (HL)+1
        case 0x35: printf("DCR      M"); break;                                                   //     1     Z, S, P, AC     (HL) <- (HL)-1
        case 0x36: printf("MVI      M, #$%02x", opcode[1]); opbytes=2; break;                       //D8   2          (HL) <- byte 2
        case 0x37: printf("STC"); break;                                                          //     1     CY     CY = 1
        case 0x38: printf("NOP"); break;               
        case 0x39: printf("DAD      SP"); break;                                                  //     1     CY     HL = HL + SP
        case 0x3a: printf("LDA      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          A <- (adr)
        case 0x3b: printf("DCX      SP"); break;                                                  //     1          SP = SP-1
        case 0x3c: printf("INR      A"); break;                                                   //     1     Z, S, P, AC     A <- A+1
        case 0x3d: printf("DCR      A"); break;                                                   //     1     Z, S, P, AC     A <- A-1
        case 0x3e: printf("MVI      A, #$%02x", opcode[1]); opbytes=2; break;                       //D8   2          A <- byte 2
        case 0x3f: printf("CMC"); break;                                                          //     1          CY     CY=!CY
        case 0x40: printf("MOV      B, B"); break;                                                //     1          B <- B
        case 0x41: printf("MOV      B, C"); break;                                                //     1          B <- C
        case 0x42: printf("MOV      B, D"); break;                                                //     1          B <- D
        case 0x43: printf("MOV      B, E"); break;                                                //     1          B <- E
        case 0x44: printf("MOV      B, H"); break;                                                //     1          B <- H
        case 0x45: printf("MOV      B, L"); break;                                                //     1          B <- L
        case 0x46: printf("MOV      B, M"); break;                                                //     1          B <- (HL)
        case 0x47: printf("MOV      B, A"); break;                                                //     1          B <- A
        case 0x48: printf("MOV      C, B"); break;                                                //     1          C <- B
        case 0x49: printf("MOV      C, C"); break;                                                //     1          C <- C
        case 0x4a: printf("MOV      C, D"); break;                                                //     1          C <- D
        case 0x4b: printf("MOV      C, E"); break;                                                //     1          C <- E
        case 0x4c: printf("MOV      C, H"); break;                                                //     1          C <- H
        case 0x4d: printf("MOV      C, L"); break;                                                //     1          C <- L
        case 0x4e: printf("MOV      C, M"); break;                                                //     1          C <- (HL)
        case 0x4f: printf("MOV      C, A"); break;                                                //     1          C <- A
        case 0x50: printf("MOV      D, B"); break;                                                //     1          D <- B
        case 0x51: printf("MOV      D, C"); break;                                                //     1          D <- C
        case 0x52: printf("MOV      D, D"); break;                                                //     1          D <- D
        case 0x53: printf("MOV      D, E"); break;                                                //     1          D <- E
        case 0x54: printf("MOV      D, H"); break;                                                //     1          D <- H
        case 0x55: printf("MOV      D, L"); break;                                                //     1          D <- L
        case 0x56: printf("MOV      D, M"); break;                                                //     1          D <- (HL)
        case 0x57: printf("MOV      D, A"); break;                                                //     1          D <- A
        case 0x58: printf("MOV      E, B"); break;                                                //     1          E <- B
        case 0x59: printf("MOV      E, C"); break;                                                //     1          E <- C
        case 0x5a: printf("MOV      E, D"); break;                                                //     1          E <- D
        case 0x5b: printf("MOV      E, E"); break;                                                //     1          E <- E
        case 0x5c: printf("MOV      E, H"); break;                                                //     1          E <- H
        case 0x5d: printf("MOV      E, L"); break;                                                //     1          E <- L
        case 0x5e: printf("MOV      E, M"); break;                                                //     1          E <- (HL)
        case 0x5f: printf("MOV      E, A"); break;                                                //     1          E <- A
        case 0x60: printf("MOV      H, B"); break;                                                //     1          H <- B
        case 0x61: printf("MOV      H, C"); break;                                                //     1          H <- C
        case 0x62: printf("MOV      H, D"); break;                                                //     1          H <- D
        case 0x63: printf("MOV      H, E"); break;                                                //     1          H <- E
        case 0x64: printf("MOV      H, H"); break;                                                //     1          H <- H
        case 0x65: printf("MOV      H, L"); break;                                                //     1          H <- L
        case 0x66: printf("MOV      H, M"); break;                                                //     1          H <- (HL)
        case 0x67: printf("MOV      H, A"); break;                                                //     1          H <- A
        case 0x68: printf("MOV      L, B"); break;                                                //     1          L <- B
        case 0x69: printf("MOV      L, C"); break;                                                //     1          L <- C
        case 0x6a: printf("MOV      L, D"); break;                                                //     1          L <- D
        case 0x6b: printf("MOV      L, E"); break;                                                //     1          L <- E
        case 0x6c: printf("MOV      L, H"); break;                                                //     1          L <- H
        case 0x6d: printf("MOV      L, L"); break;                                                //     1          L <- L
        case 0x6e: printf("MOV      L, M"); break;                                                //     1          L <- (HL)
        case 0x6f: printf("MOV      L, A"); break;                                                //     1          L <- A
        case 0x70: printf("MOV      M, B"); break;                                                //     1          (HL) <- B
        case 0x71: printf("MOV      M, C"); break;                                                //     1          (HL) <- C
        case 0x72: printf("MOV      M, D"); break;                                                //     1          (HL) <- D
        case 0x73: printf("MOV      M, E"); break;                                                //     1          (HL) <- E
        case 0x74: printf("MOV      M, H"); break;                                                //     1          (HL) <- H
        case 0x75: printf("MOV      M, L"); break;                                                //     1          (HL) <- L
        case 0x76: printf("HLT"); break;                                                          //     1          special
        case 0x77: printf("MOV      M, A"); break;                                                //     1          (HL) <- C
        case 0x78: printf("MOV      A, B"); break;                                                //     1          A <- B
        case 0x79: printf("MOV      A, C"); break;                                                //     1          A <- C
        case 0x7a: printf("MOV      A, D"); break;                                                //     1          A <- D
        case 0x7b: printf("MOV      A, E"); break;                                                //     1          A <- E
        case 0x7c: printf("MOV      A, H"); break;                                                //     1          A <- H
        case 0x7d: printf("MOV      A, L"); break;                                                //     1          A <- L
        case 0x7e: printf("MOV      A, M"); break;                                                //     1          A <- (HL)
        case 0x7f: printf("MOV      A, A"); break;                                                //     1          A <- A
        case 0x80: printf("ADD      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + B
        case 0x81: printf("ADD      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + C
        case 0x82: printf("ADD      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + D
        case 0x83: printf("ADD      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + E
        case 0x84: printf("ADD      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + H
        case 0x85: printf("ADD      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + L
        case 0x86: printf("ADD      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + (HL)
        case 0x87: printf("ADD      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + A
        case 0x88: printf("ADC      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + B + CY
        case 0x89: printf("ADC      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + C + CY
        case 0x8a: printf("ADC      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + D + CY
        case 0x8b: printf("ADC      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + E + CY
        case 0x8c: printf("ADC      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + H + CY
        case 0x8d: printf("ADC      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + L + CY
        case 0x8e: printf("ADC      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + (HL) + CY
        case 0x8f: printf("ADC      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + A + CY
        case 0x90: printf("SUB      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - B
        case 0x91: printf("SUB      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - C
        case 0x92: printf("SUB      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + D
        case 0x93: printf("SUB      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - E
        case 0x94: printf("SUB      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + H
        case 0x95: printf("SUB      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - L
        case 0x96: printf("SUB      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + (HL)
        case 0x97: printf("SUB      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - A
        case 0x98: printf("SBB      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - B - CY
        case 0x99: printf("SBB      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - C - CY
        case 0x9a: printf("SBB      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - D - CY
        case 0x9b: printf("SBB      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - E - CY
        case 0x9c: printf("SBB      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - H - CY
        case 0x9d: printf("SBB      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - L - CY
        case 0x9e: printf("SBB      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - (HL) - CY
        case 0x9f: printf("SBB      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - A - CY
        case 0xa0: printf("ANA      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & B
        case 0xa1: printf("ANA      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & C
        case 0xa2: printf("ANA      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & D
        case 0xa3: printf("ANA      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & E
        case 0xa4: printf("ANA      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & H
        case 0xa5: printf("ANA      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & L
        case 0xa6: printf("ANA      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & (HL)
        case 0xa7: printf("ANA      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & A
        case 0xa8: printf("XRA      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ B
        case 0xa9: printf("XRA      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ C
        case 0xaa: printf("XRA      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ D
        case 0xab: printf("XRA      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ E
        case 0xac: printf("XRA      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ H
        case 0xad: printf("XRA      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ L
        case 0xae: printf("XRA      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ (HL)
        case 0xaf: printf("XRA      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ A
        case 0xb0: printf("ORA      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | B
        case 0xb1: printf("ORA      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | C
        case 0xb2: printf("ORA      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | D
        case 0xb3: printf("ORA      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | E
        case 0xb4: printf("ORA      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | H
        case 0xb5: printf("ORA      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | L
        case 0xb6: printf("ORA      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | (HL)
        case 0xb7: printf("ORA      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | A
        case 0xb8: printf("CMP      B"); break;                                                   //     1     Z, S, P, CY, AC     A - B
        case 0xb9: printf("CMP      C"); break;                                                   //     1     Z, S, P, CY, AC     A - C
        case 0xba: printf("CMP      D"); break;                                                   //     1     Z, S, P, CY, AC     A - D
        case 0xbb: printf("CMP      E"); break;                                                   //     1     Z, S, P, CY, AC     A - E
        case 0xbc: printf("CMP      H"); break;                                                   //     1     Z, S, P, CY, AC     A - H
        case 0xbd: printf("CMP      L"); break;                                                   //     1     Z, S, P, CY, AC     A - L
        case 0xbe: printf("CMP      M"); break;                                                   //     1     Z, S, P, CY, AC     A - (HL)
        case 0xbf: printf("CMP      A"); break;                                                   //     1     Z, S, P, CY, AC     A - A
        case 0xc0: printf("RNZ"); break;                                                          //     1     if NZ, RET
        case 0xc1: printf("POP      B"); break;                                                   //     1     C <- (sp); B <- (sp+1); sp <- sp+2
        case 0xc2: printf("JNZ      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NZ, PC <- adr
        case 0xc3: printf("JMP      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          PC <= adr
        case 0xc4: printf("CNZ      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NZ, CALL adr
        case 0xc5: printf("PUSH     B"); break;                                                   //     1          (sp-2)<-C; (sp-1)<-B; sp <- sp - 2
        case 0xc6: printf("ADI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A + byte
        case 0xc7: printf("RST      0"); break;                                                   //     1          CALL $0
        case 0xc8: printf("RZ"); break;                                                           //     1          if Z, RET
        case 0xc9: printf("RET"); break;                                                          //     1          PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
        case 0xca: printf("JZ       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if Z, PC <- adr
        case 0xcb: printf("NOP"); break;
        case 0xcc: printf("CZ       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if Z, CALL adr
        case 0xcd: printf("CALL     #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP+2;PC=adr
        case 0xce: printf("ACI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A + data + CY
        case 0xcf: printf("RST      1"); break;                                                   //     1          CALL $8
        case 0xd0: printf("RNC"); break;                                                          //     1          if NCY, RET
        case 0xd1: printf("POP      D"); break;                                                   //     1          E <- (sp); D <- (sp+1); sp <- sp+2
        case 0xd2: printf("JNC      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NCY, PC<-adr
        case 0xd3: printf("OUT      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2          special
        case 0xd4: printf("CNC      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NCY, CALL adr
        case 0xd5: printf("PUSH     D"); break;                                                   //     1          (sp-2)<-E; (sp-1)<-D; sp <- sp - 2
        case 0xd6: printf("SUI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A - data
        case 0xd7: printf("RST      2"); break;                                                   //     1          CALL $10
        case 0xd8: printf("RC"); break;                                                           //     1          if CY, RET
        case 0xd9: printf("NOP"); break;          
        case 0xda: printf("JC       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if CY, PC<-adr
        case 0xdb: printf("IN       #$%02x", opcode[1]); opbytes=2; break;                          //D8   2          special
        case 0xdc: printf("CC       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if CY, CALL adr
        case 0xdd: printf("NOP"); break;               
        case 0xde: printf("SBI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A - data - CY
        case 0xdf: printf("RST      3"); break;                                                   //     1          CALL $18
        case 0xe0: printf("RPO"); break;                                                          //     1          if PO, RET
        case 0xe1: printf("POP      H"); break;                                                   //     1          L <- (sp); H <- (sp+1); sp <- sp+2
        case 0xe2: printf("JPO      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PO, PC <- adr
        case 0xe3: printf("XTHL"); break;                                                         //     1          L <-> (SP); H <-> (SP+1)
        case 0xe4: printf("CPO      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PO, CALL adr
        case 0xe5: printf("PUSH     H"); break;                                                   //     1          (sp-2)<-L; (sp-1)<-H; sp <- sp - 2
        case 0xe6: printf("ANI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A & data
        case 0xe7: printf("RST      4"); break;                                                   //     1          CALL $20
        case 0xe8: printf("RPE"); break;                                                          //     1          if PE, RET
        case 0xe9: printf("PCHL"); break;                                                         //     1          PC.hi <- H; PC.lo <- L
        case 0xea: printf("JPE      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PE, PC <- adr
        case 0xeb: printf("XCHG"); break;                                                         //     1          H <-> D; L <-> E
        case 0xec: printf("CPE      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PE, CALL adr
        case 0xed: printf("NOP"); break;
        case 0xee: printf("XRI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A ^ data
        case 0xef: printf("RST      5"); break;                                                   //     1          CALL $28
        case 0xf0: printf("RP"); break;                                                           //     1          if P, RET
        case 0xf1: printf("POP      PSW"); break;                                                 //     1          flags <- (sp); A <- (sp+1); sp <- sp+2
        case 0xf2: printf("JP       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if P=1 PC <- adr
        case 0xf3: printf("DI"); break;                                                           //     1          special
        case 0xf4: printf("CP       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if P, PC <- adr
        case 0xf5: printf("PUSH     PSW"); break;                                                 //     1          (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
        case 0xf6: printf("ORI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A | data
        case 0xf7: printf("RST      6"); break;                                                   //     1          CALL $30
        case 0xf8: printf("RM"); break;                                                           //     1          if M, RET
        case 0xf9: printf("SPHL"); break;                                                         //     1          SP=HL
        case 0xfa: printf("JM       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if M, PC <- adr
        case 0xfb: printf("EI"); break;                                                           //     1          special
        case 0xfc: printf("CM       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if M, CALL adr
        case 0xfd: printf("NOP"); break;
        case 0xfe: printf("CPI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A - data
        case 0xff: printf("RST      7"); break;                                                   //     1          CALL $38
        default: printf("Unknown code.\n"); return 0;
    }
    printf("\n");
    return opbytes;
}

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

int emulateCpu8080Op(Cpu8080* cpu, const uint8_t* code)
{
    struct timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);
    const uint8_t* opcode;
    if (code == NULL) opcode = &cpu->memory[cpu->pc];
    else opcode = code;
#ifdef PRINT_OPS
    disassembleCpu8080Op(cpu);
#endif
    switch (*opcode)
    {
    case 0x00 : cpu->pc += 1; break;						// NOP
    case 0x01 : cpu->b = opcode[2]; cpu->c = opcode[1]; cpu->pc += 3; break;		// LXI B,D16
    case 0x02 :						// STAX B
    {
        writeCpu8080Memory(cpu, ((uint16_t)cpu->b) << 8 | ((uint16_t) cpu->c), cpu->a);
        cpu->pc += 1;
        break;
    }
    case 0x03 :								// INX B
    {
        uint16_t w = (uint16_t) cpu->b;
        w = (w << 8) | cpu->c; w++;
        cpu->c = w & 0xff;
        cpu->b = (w >> 8) & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x04 : cpu->b++; updateCpu8080Flags(cpu, cpu->b); cpu->pc += 1; break;			// INR B
    case 0x05 : cpu->b--; updateCpu8080Flags(cpu, cpu->b); cpu->pc += 1; break;			// DCR B
    case 0x06 : cpu->b = opcode[1]; cpu->pc += 2; break;				// MVI B,D8
    case 0x07 :						// RLC
    {
        uint8_t prev_bit_7 = (cpu->a >> 7) & 0x01;
        cpu->a = cpu->a << 1;
        cpu->a = cpu->a | prev_bit_7;
        cpu->flags.c = prev_bit_7;
        cpu->pc += 1;
        break;
    }
    case 0x08 : cpu->pc += 1; break;						// NOP
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
        cpu->pc += 1;
        break;
    }
    case 0x0a :						// LDAX B
    {
        cpu->a = readCpu8080Memory(cpu, ((uint16_t)cpu->b) << 8 | ((uint16_t) cpu->c));
        cpu->pc += 1;
        break;
    }
    case 0x0b :								// DCX B
    {
        uint16_t w = (uint16_t) cpu->b;
        w = (w << 8) | cpu->c; w--;
        cpu->c = w & 0xff;
        cpu->b = (w >> 8) & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x0c : cpu->c++; updateCpu8080Flags(cpu, cpu->c); cpu->pc += 1; break;			// INR C
    case 0x0d : cpu->c--; updateCpu8080Flags(cpu, cpu->c); cpu->pc += 1; break;			// DCR C
    case 0x0e : cpu->c = opcode[1]; cpu->pc += 2; break;				// MVI C,D8
    case 0x0f :						// RRC
    {
        uint8_t prev_bit_0 = cpu->a & 0x01;
        cpu->a = cpu->a >> 1;
        cpu->a = cpu->a | (prev_bit_0 << 7);
        cpu->flags.c = prev_bit_0;
        cpu->pc += 1;
        break;
    }
    case 0x10 : cpu->pc += 1; break;						// NOP
    case 0x11 : cpu->d = opcode[2]; cpu->e = opcode[1]; cpu->pc += 3; break;		// LXI D,D16
    case 0x12 :						// STAX D
    {
        writeCpu8080Memory(cpu, ((uint16_t)cpu->d) << 8 | ((uint16_t) cpu->e), cpu->a);
        cpu->pc += 1;
        break;
    }
    case 0x13 :								// INX D
    {
        uint16_t w = (uint16_t) cpu->d;
        w = (w << 8) | cpu->e; w++;
        cpu->e = w & 0xff;
        cpu->d = (w >> 8) & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x14 : cpu->d++; updateCpu8080Flags(cpu, cpu->d); cpu->pc += 1; break;			// INR D
    case 0x15 : cpu->d--; updateCpu8080Flags(cpu, cpu->d); cpu->pc += 1; break;			// DCR D
    case 0x16 : cpu->d = opcode[1]; cpu->pc += 2; break;				// MVI D,D8
    case 0x17 :						// RAL
    {
        uint8_t prev_bit_7 = (cpu->a >> 7) & 0x01;
        cpu->a = cpu->a << 1;
        cpu->a = cpu->a | cpu->flags.c;
        cpu->flags.c = prev_bit_7;
        cpu->pc += 1;
        break;
    }
    case 0x18 : cpu->pc += 1; break;						// NOP
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
        cpu->pc += 1;
        break;
    }
    case 0x1a :						// LDAX D
    {
        cpu->a = readCpu8080Memory(cpu, ((uint16_t)cpu->d) << 8 | ((uint16_t) cpu->e));
        cpu->pc += 1;
        break;
    }
    case 0x1b :								// DCX D
    {
        uint16_t w = (uint16_t) cpu->d;
        w = (w << 8) | cpu->e; w--;
        cpu->e = w & 0xff;
        cpu->d = (w >> 8) & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x1c : cpu->e++; updateCpu8080Flags(cpu, cpu->e); cpu->pc += 1; break;			// INR E
    case 0x1d : cpu->e--; updateCpu8080Flags(cpu, cpu->e); cpu->pc += 1; break;			// DCR E
    case 0x1e : cpu->e = opcode[1]; cpu->pc += 2; break;				// MVI E,D8
    case 0x1f :						// RAR
    {
        uint8_t prev_bit_0 = cpu->a & 0x01;
        cpu->a = cpu->a >> 1;
        cpu->a = cpu->a | (cpu->flags.c << 7);
        cpu->flags.c = prev_bit_0;
        cpu->pc += 1;
        break;
    }
    case 0x20 : cpu->pc += 1; break;						// NOP
    case 0x21 : cpu->h = opcode[2]; cpu->l = opcode[1]; cpu->pc += 3; break;		// LXI H,D16
    case 0x22 :						// SHLD adr
    {
        uint16_t mem_offset = opcode[2];
        mem_offset = (mem_offset << 8) | opcode[1];
        writeCpu8080Memory(cpu, mem_offset, cpu->l);
        writeCpu8080Memory(cpu, mem_offset + 1, cpu->h);
        cpu->pc += 3;
        break;
    }
    case 0x23 :								// INX H
    {
        uint16_t w = (uint16_t) cpu->h;
        w = (w << 8) | cpu->l; w++;
        cpu->l = w & 0xff;
        cpu->h = (w >> 8) & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x24 : cpu->h++; updateCpu8080Flags(cpu, cpu->h); cpu->pc += 1; break;			// INR H
    case 0x25 : cpu->h--; updateCpu8080Flags(cpu, cpu->h); cpu->pc += 1; break;			// DCR H
    case 0x26 : cpu->h = opcode[1]; cpu->pc += 2; break;				// MVI H,D8
    case 0x27 :						// DAA
    {
        cpu8080DAA(cpu);
        cpu->pc += 1;
        break;
    }
    case 0x28 : cpu->pc += 1; break;						// NOP
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
        cpu->pc += 1;
        break;
    }
    case 0x2a :						// LHLD adr
    {
        uint16_t mem_offset = opcode[2];
        mem_offset = (mem_offset << 8) | opcode[1];
        cpu->l = readCpu8080Memory(cpu, mem_offset);
        cpu->h = readCpu8080Memory(cpu, mem_offset+1);
        cpu->pc += 3;
        break;
    }
    case 0x2b :								// DCX H
    {
        uint16_t w = (uint16_t) cpu->h;
        w = (w << 8) | cpu->l; w--;
        cpu->l = w & 0xff;
        cpu->h = (w >> 8) & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x2c : cpu->l++; updateCpu8080Flags(cpu, cpu->l); cpu->pc += 1; break;			// INR L
    case 0x2d : cpu->l--; updateCpu8080Flags(cpu, cpu->l); cpu->pc += 1; break;			// DCR L
    case 0x2e : cpu->l = opcode[1]; cpu->pc += 2; break;				// MVI L, D8
    case 0x2f : cpu->a = ~cpu->a; cpu->pc += 1; break;				// CMA
    case 0x30 : cpu->pc += 1; break;						// NOP
    case 0x31 :						// LXI SP, D16
    {
        uint16_t h = opcode[2]; uint16_t l = opcode[1];
        cpu->sp = h << 8 | l;
        cpu->pc += 3;
        break;
    }
    case 0x32 :						// STA adr
    {
        writeCpu8080Memory(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]), cpu->a);
        cpu->pc += 3;
        break;
    }
    case 0x33 : cpu->sp++; cpu->pc += 1; break;				// INX SP
    case 0x34 : writeCpu8080MemoryAtHL(cpu, readCpu8080MemoryAtHL(cpu) + 1); updateCpu8080Flags(cpu, readCpu8080MemoryAtHL(cpu)); cpu->pc += 1; break;	// INR M
    case 0x35 : writeCpu8080MemoryAtHL(cpu, readCpu8080MemoryAtHL(cpu) - 1); updateCpu8080Flags(cpu, readCpu8080MemoryAtHL(cpu)); cpu->pc += 1; break;	// DCR M
    case 0x36 : writeCpu8080MemoryAtHL(cpu, opcode[1]); cpu->pc += 2; break;		// MVI M,D8
    case 0x37 : cpu->flags.c = 0b1; cpu->pc += 1; break;				// STC
    case 0x38 : cpu->pc += 1; break;						// NOP
    case 0x39 :								// DAD SP
    {
        uint16_t h = (uint16_t) cpu->h;
        uint16_t l = (uint16_t) cpu->l;
        uint16_t hl = h << 8 | l;
        uint32_t r = (uint32_t) hl + (uint32_t) cpu->sp;
        cpu->flags.c = (r > 0xffff);
        cpu->l = (uint8_t) r & 0xff;
        cpu->h = (uint8_t) r >> 8 & 0xff;
        cpu->pc += 1;
        break;
    }
    case 0x3a :						// LDA adr
    {
        cpu->a = readCpu8080Memory(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]));
        cpu->pc += 3;
        break;
    }
    case 0x3b : cpu->sp--; cpu->pc += 1; break;				// DCX SP
    case 0x3c : cpu->a++; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// INR A
    case 0x3d : cpu->a--; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// DCR A
    case 0x3e : cpu->a = opcode[1]; cpu->pc += 2; break;				// MVI A,D8
    case 0x3f : cpu->flags.c = ~cpu->flags.c; cpu->pc += 1; break;			// CMC
    case 0x40 : cpu->b = cpu->b; cpu->pc += 1; break;				// MOV B,B
    case 0x41 : cpu->b = cpu->c; cpu->pc += 1; break;				// MOV B,C
    case 0x42 : cpu->b = cpu->d; cpu->pc += 1; break;				// MOV B,D
    case 0x43 : cpu->b = cpu->e; cpu->pc += 1; break;				// MOV B,E
    case 0x44 : cpu->b = cpu->h; cpu->pc += 1; break;				// MOV B,H
    case 0x45 : cpu->b = cpu->l; cpu->pc += 1; break;				// MOV B,L
    case 0x46 : cpu->b = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV B,M
    case 0x47 : cpu->b = cpu->a; cpu->pc += 1; break;				// MOV B,A
    case 0x48 : cpu->c = cpu->b; cpu->pc += 1; break;				// MOV C,B
    case 0x49 : cpu->c = cpu->c; cpu->pc += 1; break;				// MOV C,C
    case 0x4a : cpu->c = cpu->d; cpu->pc += 1; break;				// MOV C,D
    case 0x4b : cpu->c = cpu->e; cpu->pc += 1; break;				// MOV C,E
    case 0x4c : cpu->c = cpu->h; cpu->pc += 1; break;				// MOV C,H
    case 0x4d : cpu->c = cpu->l; cpu->pc += 1; break;				// MOV C,L
    case 0x4e : cpu->c = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV C,M
    case 0x4f : cpu->c = cpu->a; cpu->pc += 1; break;				// MOV C,A
    case 0x50 : cpu->d = cpu->b; cpu->pc += 1; break;				// MOV D,B
    case 0x51 : cpu->d = cpu->c; cpu->pc += 1; break;				// MOV D,C
    case 0x52 : cpu->d = cpu->d; cpu->pc += 1; break;				// MOV D,D
    case 0x53 : cpu->d = cpu->e; cpu->pc += 1; break;				// MOV D,E
    case 0x54 : cpu->d = cpu->h; cpu->pc += 1; break;				// MOV D,H
    case 0x55 : cpu->d = cpu->l; cpu->pc += 1; break;				// MOV D,L
    case 0x56 : cpu->d = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV D,M
    case 0x57 : cpu->d = cpu->a; cpu->pc += 1; break;				// MOV D,A
    case 0x58 : cpu->e = cpu->b; cpu->pc += 1; break;				// MOV E,B
    case 0x59 : cpu->e = cpu->c; cpu->pc += 1; break;				// MOV E,C
    case 0x5a : cpu->e = cpu->d; cpu->pc += 1; break;				// MOV E,D
    case 0x5b : cpu->e = cpu->e; cpu->pc += 1; break;				// MOV E,E
    case 0x5c : cpu->e = cpu->h; cpu->pc += 1; break;				// MOV E,H
    case 0x5d : cpu->e = cpu->l; cpu->pc += 1; break;				// MOV E,L
    case 0x5e : cpu->e = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV E,M
    case 0x5f : cpu->e = cpu->a; cpu->pc += 1; break;				// MOV E,A
    case 0x60 : cpu->h = cpu->b; cpu->pc += 1; break;				// MOV H,B
    case 0x61 : cpu->h = cpu->c; cpu->pc += 1; break;				// MOV H,C
    case 0x62 : cpu->h = cpu->d; cpu->pc += 1; break;				// MOV H,D
    case 0x63 : cpu->h = cpu->e; cpu->pc += 1; break;				// MOV H,E
    case 0x64 : cpu->h = cpu->h; cpu->pc += 1; break;				// MOV H,H
    case 0x65 : cpu->h = cpu->l; cpu->pc += 1; break;				// MOV H,L
    case 0x66 : cpu->h = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV H,M
    case 0x67 : cpu->h = cpu->a; cpu->pc += 1; break;				// MOV H,A
    case 0x68 : cpu->l = cpu->b; cpu->pc += 1; break;				// MOV L,B
    case 0x69 : cpu->l = cpu->c; cpu->pc += 1; break;				// MOV L,C
    case 0x6a : cpu->l = cpu->d; cpu->pc += 1; break;				// MOV L,D
    case 0x6b : cpu->l = cpu->e; cpu->pc += 1; break;				// MOV L,E
    case 0x6c : cpu->l = cpu->h; cpu->pc += 1; break;				// MOV L,H
    case 0x6d : cpu->l = cpu->l; cpu->pc += 1; break;				// MOV L,L
    case 0x6e : cpu->l = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV L,M
    case 0x6f : cpu->l = cpu->a; cpu->pc += 1; break;				// MOV L,A
    case 0x70 : writeCpu8080MemoryAtHL(cpu, cpu->b); cpu->pc += 1; break;			// MOV M,B
    case 0x71 : writeCpu8080MemoryAtHL(cpu, cpu->c); cpu->pc += 1; break;			// MOV M,C
    case 0x72 : writeCpu8080MemoryAtHL(cpu, cpu->d); cpu->pc += 1; break;			// MOV M,D
    case 0x73 : writeCpu8080MemoryAtHL(cpu, cpu->e); cpu->pc += 1; break;			// MOV M,E
    case 0x74 : writeCpu8080MemoryAtHL(cpu, cpu->h); cpu->pc += 1; break;			// MOV M,H
    case 0x75 : writeCpu8080MemoryAtHL(cpu, cpu->l); cpu->pc += 1; break;			// MOV M,L
    case 0x76 : cpu->pc += 1; cpu->stopped = 1; cpu->interrupt_enabled=1; break;		// HLT
    case 0x77 : writeCpu8080MemoryAtHL(cpu, cpu->a); cpu->pc += 1; break;			// MOV M,A
    case 0x78 : cpu->a = cpu->b; cpu->pc += 1; break;				// MOV A,B
    case 0x79 : cpu->a = cpu->c; cpu->pc += 1; break;				// MOV A,C
    case 0x7a : cpu->a = cpu->d; cpu->pc += 1; break;				// MOV A,D
    case 0x7b : cpu->a = cpu->e; cpu->pc += 1; break;				// MOV A,E
    case 0x7c : cpu->a = cpu->h; cpu->pc += 1; break;				// MOV A,H
    case 0x7d : cpu->a = cpu->l; cpu->pc += 1; break;				// MOV A,L
    case 0x7e : cpu->a = readCpu8080MemoryAtHL(cpu); cpu->pc += 1; break;			// MOV A,M
    case 0x7f : cpu->a = cpu->a; cpu->pc += 1; break;				// MOV A,A
    case 0x80 :									// ADD B
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->b);
        cpu->pc += 1;
        break;
    }
    case 0x81 :									// ADD C
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->c);
        cpu->pc += 1;
        break;
    }
    case 0x82 :									// ADD D
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->d);
        cpu->pc += 1;
        break;
    }
    case 0x83 :									// ADD E
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->e);
        cpu->pc += 1;
        break;
    }
    case 0x84 :									// ADD H
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->h);
        cpu->pc += 1;
        break;
    }
    case 0x85 :									// ADD L
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->l);
        cpu->pc += 1;
        break;
    }
    case 0x86 :									// ADD M
    {
        cpu->a = cpu8080Add(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));
        cpu->pc += 1;
        break;
    }
    case 0x87 :									// ADD A
    {
        cpu->a = cpu8080Add(cpu, cpu->a, cpu->a);
        cpu->pc += 1;
        break;
    }
    case 0x88 :									// ADC B
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->b);
        cpu->pc += 1;
        break;
    }
    case 0x89 :									// ADC C
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->c);
        cpu->pc += 1;
        break;
    }
    case 0x8a :									// ADC D
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->d);
        cpu->pc += 1;
        break;
    }
    case 0x8b :									// ADC E
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->e);
        cpu->pc += 1;
        break;
    }
    case 0x8c :									// ADC H
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->h);
        cpu->pc += 1;
        break;
    }
    case 0x8d :									// ADC L
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->l);
        cpu->pc += 1;
        break;
    }
    case 0x8e :									// ADC M
    {
        uint16_t r = (uint16_t) cpu->a + (uint16_t)  + (uint16_t) cpu->flags.c;
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));
        cpu->pc += 1;
        break;
    }
    case 0x8f :									// ADC A
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->a);
        cpu->pc += 1;
        break;
    }
    case 0x90 :									// SUB B
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->b);
        cpu->pc += 1;
        break;
    }
    case 0x91 :									// SUB C
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->c);
        cpu->pc += 1;
        break;
    }
    case 0x92 :									// SUB D
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->d);
        cpu->pc += 1;
        break;
    }
    case 0x93 :									// SUB E
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->e);
        cpu->pc += 1;
        break;
    }
    case 0x94 :									// SUB H
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->h);
        cpu->pc += 1;
        break;
    }
    case 0x95 :									// SUB L
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->l);
        cpu->pc += 1;
        break;
    }
    case 0x96 :									// SUB M
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));
        cpu->pc += 1;
        break;
    }
    case 0x97 :									// SUB A
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, cpu->a);
        cpu->pc += 1;
        break;
    }
    case 0x98 :									// SBB B
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->b);
        cpu->pc += 1;
        break;
    }
    case 0x99 :									// SBB C
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->c);
        cpu->pc += 1;
        break;
    }
    case 0x9a :									// SBB D
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->d);
        cpu->pc += 1;
        break;
    }
    case 0x9b :									// SBB E
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->e);
        cpu->pc += 1;
        break;
    }
    case 0x9c :									// SBB H
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->h);
        cpu->pc += 1;
        break;
    }
    case 0x9d :									// SBB L
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->l);
        cpu->pc += 1;
        break;
    }
    case 0x9e :									// SBB M
    {
        uint16_t r = (uint16_t) cpu->a - (uint16_t) readCpu8080MemoryAtHL(cpu) - (uint16_t) cpu->flags.c;
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));
        cpu->pc += 1;
        break;
    }
    case 0x9f :									// SBB A
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->a);
        cpu->pc += 1;
        break;
    }
    case 0xa0 : cpu->a &= cpu->b; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA B
    case 0xa1 : cpu->a &= cpu->c; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA C
    case 0xa2 : cpu->a &= cpu->d; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA D
    case 0xa3 : cpu->a &= cpu->e; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA E
    case 0xa4 : cpu->a &= cpu->h; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA H
    case 0xa5 : cpu->a &= cpu->l; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA L
    case 0xa6 : cpu->a &= readCpu8080MemoryAtHL(cpu); updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;		// ANA M
    case 0xa7 : cpu->a &= cpu->a; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ANA A
    case 0xa8 : cpu->a ^= cpu->b; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA B
    case 0xa9 : cpu->a ^= cpu->c; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA C
    case 0xaa : cpu->a ^= cpu->d; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA D
    case 0xab : cpu->a ^= cpu->e; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA E
    case 0xac : cpu->a ^= cpu->h; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA H
    case 0xad : cpu->a ^= cpu->l; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA L
    case 0xae : cpu->a ^= readCpu8080MemoryAtHL(cpu); updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;		// XRA M
    case 0xaf : cpu->a ^= cpu->a; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// XRA A
    case 0xb0 : cpu->a |= cpu->b; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA B
    case 0xb1 : cpu->a |= cpu->c; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA C
    case 0xb2 : cpu->a |= cpu->d; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA D
    case 0xb3 : cpu->a |= cpu->e; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA E
    case 0xb4 : cpu->a |= cpu->h; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA H
    case 0xb5 : cpu->a |= cpu->l; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA L
    case 0xb6 : cpu->a |= readCpu8080MemoryAtHL(cpu); updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;		// ORA M
    case 0xb7 : cpu->a |= cpu->a; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 1; break;			// ORA A
    case 0xb8 : cpu->flags.c = cpu->a < cpu->b; cpu->flags.z = cpu->a == cpu->b; cpu->pc += 1; break;		// CMP B
    case 0xb9 : cpu->flags.c = cpu->a < cpu->c; cpu->flags.z = cpu->a == cpu->c; cpu->pc += 1; break;		// CMP C
    case 0xba : cpu->flags.c = cpu->a < cpu->d; cpu->flags.z = cpu->a == cpu->d; cpu->pc += 1; break;		// CMP D
    case 0xbb : cpu->flags.c = cpu->a < cpu->e; cpu->flags.z = cpu->a == cpu->e; cpu->pc += 1; break;		// CMP E
    case 0xbc : cpu->flags.c = cpu->a < cpu->h; cpu->flags.z = cpu->a == cpu->h; cpu->pc += 1; break;		// CMP H
    case 0xbd : cpu->flags.c = cpu->a < cpu->l; cpu->flags.z = cpu->a == cpu->l; cpu->pc += 1; break;		// CMP L
    case 0xbe :				// CMP M
    {
        uint8_t m = readCpu8080MemoryAtHL(cpu);
        cpu->flags.c = cpu->a < m;
        cpu->flags.z = cpu->a == m;
        cpu->pc += 1;
        break;
    }
    case 0xbf : cpu->flags.c = cpu->a < cpu->a; cpu->flags.z = cpu->a == cpu->a; cpu->pc += 1; break;		// CMP A
    case 0xc0 :						// RNZ
    {
        cpu->pc += 1;
        if (cpu->flags.z) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xc1 :						// POP B
    {
        cpu->c = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->b = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc += 1;
        break;
    }
    case 0xc2 :						// JNZ adr
    {
        cpu->pc += 3;
        if (cpu->flags.z) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xc3 :						// JMP adr
    {
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xc4 :						// CNZ adr
    {
        cpu->pc += 3;
        if (cpu->flags.z) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xc5 :						// PUSH B
    {
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->b);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->c);
        cpu->pc += 1;
        break;
    }
    case 0xc6 :									// ADI D8
    {
        cpu->a = cpu8080Add(cpu, cpu->a, opcode[1]);
        cpu->pc += 2;
        break;
    }
    case 0xc7 :						// RST 0
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x0;
        break;
    }
    case 0xc8 :						// RZ
    {
        cpu->pc += 1;
        if (!cpu->flags.z) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xc9 :						// RET
    {
        uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xca :						// JZ adr
    {
        cpu->pc += 3;
        if (!cpu->flags.z) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xcb : cpu->pc += 1; break;						// NOP
    case 0xcc :						// CZ adr
    {
        cpu->pc += 3;
        if (!cpu->flags.z) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xcd :						// CALL adr
    #ifdef FOR_CPUDIAG
        if (5 ==  ((opcode[2] << 8) | opcode[1]))
        {
            if (cpu->c == 9)
            {
                uint16_t offset = (cpu->d<<8) | (cpu->e);
                char *str = &cpu->memory[offset+3];  //skip the prefix bytes
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
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xce :									// ACI D8
    {
        cpu->a = cpu8080AddWithCarry(cpu, cpu->a, opcode[1]);
        cpu->pc += 2;
        break;
    }
    case 0xcf :						// RST 1
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x8;
        break;
    }
    case 0xd0 :						// RNC
    {
        cpu->pc += 1;
        if (cpu->flags.c) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xd1 :						// POP D
    {
        cpu->e = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->d = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc += 1;
        break;
    }
    case 0xd2 :						// JNC adr
    {
        cpu->pc += 3;
        if (cpu->flags.c) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xd3 : cpu->out[opcode[1]].data = cpu->a; (*cpu->io_callbacks[opcode[1]])(cpu); cpu->pc += 2; break;		// OUT D8
    case 0xd4 :						// CNC adr
    {
        cpu->pc += 3;
        if (cpu->flags.c) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xd5 :						// PUSH D
    {
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->d);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->e);
        cpu->pc += 1;
        break;
    }
    case 0xd6 :									// SUI D8
    {
        cpu->a = cpu8080Sub(cpu, cpu->a, opcode[1]);
        cpu->pc += 2;
        break;
    }
    case 0xd7 :						// RST 2
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x10;
        break;
    }
    case 0xd8 :						// RC
    {
        cpu->pc += 1;
        if (!cpu->flags.c) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xd9 : cpu->pc += 1; break;						// NOP
    case 0xda :						// JC adr
    {
        cpu->pc += 3;
        if (!cpu->flags.c) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xdb : cpu->a = cpu->in[opcode[1]].data; cpu->pc += 2; break;		// IN D8
    case 0xdc :						// CC adr
    {
        cpu->pc += 3;
        if (!cpu->flags.c) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xdd : cpu->pc += 1; break;						// NOP
    case 0xde :									// SBI D8
    {
        cpu->a = cpu8080SubWithBorrow(cpu, cpu->a, opcode[1]);
        cpu->pc += 2;
        break;
    }
    case 0xdf :						// RST 3
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x18;
        break;
    }
    case 0xe0 :						// RPO
    {
        cpu->pc += 1;
        if (cpu->flags.p) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xe1 :						// POP H
    {
        cpu->l = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->h = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc += 1;
        break;
    }
    case 0xe2 :						// JPO adr
    {
        cpu->pc += 3;
        if (cpu->flags.p) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xe3 :						// XTHL
    {
        uint8_t l = cpu->l, h = cpu->h;
        cpu->l = readCpu8080Memory(cpu, cpu->sp);
        cpu->h = readCpu8080Memory(cpu, cpu->sp+1);
        writeCpu8080Memory(cpu, cpu->sp, l);
        writeCpu8080Memory(cpu, cpu->sp + 1, h);
        cpu->pc += 1;
        break;
    }
    case 0xe4 :						// CPO adr
    {
        cpu->pc += 3;
        if (cpu->flags.p) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xe5 :						// PUSH H
    {
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->h);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->l);
        cpu->pc += 1;
        break;
    }
    case 0xe6 : cpu->a &= opcode[1]; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 2; break;			// ANI D8
    case 0xe7 :						// RST 4
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x20;
        break;
    }
    case 0xe8 :						// RPE
    {
        cpu->pc += 1;
        if (!cpu->flags.p) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xe9 :						// PCHL
    {
        cpu->pc = cpu->h;
        cpu->pc = (cpu->pc << 8) | cpu->l;
        break;
    }
    case 0xea :						// JPE adr
    {
        cpu->pc += 3;
        if (!cpu->flags.p) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xeb :						// XCHG
    {
        uint8_t l = cpu->l, h = cpu->h;
        cpu->l = cpu->e; cpu->e = l;
        cpu->h = cpu->d; cpu->d = h;
        cpu->pc += 1;
        break;
    }
    case 0xec :						// CPE adr
    {
        cpu->pc += 3;
        if (!cpu->flags.p) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xed : cpu->pc += 1; break;						// NOP
    case 0xee : cpu->a ^= opcode[1]; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 2; break;			// XRI D8
    case 0xef :						// RST 5
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x28;
        break;
    }
    case 0xf0 :						// RP
    {
        cpu->pc += 1;
        if (cpu->flags.s) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xf1 :						// POP PSW
    {
        *((uint8_t*) &cpu->flags) = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->a = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc += 1;
        break;
    }
    case 0xf2 :						// JP adr
    {
        cpu->pc += 3;
        if (cpu->flags.s) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xf3 : cpu->interrupt_enabled = 0; cpu->pc += 1; break;				// DI

    case 0xf4 :						// CP adr
    {
        cpu->pc += 3;
        if (cpu->flags.s) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xf5 :						// PUSH PSW
    {
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->a);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, *((uint8_t*) &cpu->flags));
        cpu->pc += 1;
        break;
    }
    case 0xf6 : cpu->a |= opcode[1]; updateCpu8080Flags(cpu, cpu->a); cpu->pc += 2; break;			// ORI D8
    case 0xf7 :						// RST 6
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x30;
        break;
    }
    case 0xf8 :						// RM
    {
        cpu->pc += 1;
        if (!cpu->flags.s) break;	uint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        uint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;
        cpu->pc = (pch << 8) | pcl;
        break;
    }
    case 0xf9 :						// SPHL
    {
        cpu->sp = cpu->h;
        cpu->sp = (cpu->sp << 8) | cpu->l;
        cpu->pc += 1;
        break;
    }
    case 0xfa :						// JM adr
    {
        cpu->pc += 3;
        if (!cpu->flags.s) break;
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xfb : cpu->interrupt_enabled = 1; cpu->pc += 1; break;				// EI

    case 0xfc :						// CM adr
    {
        cpu->pc += 3;
        if (!cpu->flags.s) break;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = opcode[2];
        cpu->pc = (cpu->pc << 8) | opcode[1];
        break;
    }
    case 0xfd : cpu->pc += 1; break;						// NOP
    case 0xfe : cpu->flags.c = cpu->a < opcode[1]; cpu->flags.z = cpu->a == opcode[1]; cpu->pc += 2; break;	// CPI D8
    case 0xff :						// RST 7
    {
        cpu->pc += 1;
        uint8_t pcl = cpu->pc & 0xff;
        uint8_t pch = (cpu->pc >> 8) & 0xff;
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);
        cpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);
        cpu->pc = 0x38;
        break;
    }
    default:
        break;
    }
#ifdef PRINT_STATE
    printCpu8080State(cpu);
#endif
    /// Emulate at 8080 CPU clock rate
    struct timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);
    while(end_time.tv_nsec - start_time.tv_nsec < cpu->nsec_per_cycle * cpu8080Cycles[*opcode])
        clock_gettime(CLOCK_REALTIME, &end_time);
    return cpu8080Cycles[*opcode];
}

int emulateCpu8080(Cpu8080* cpu)
{
    pthread_mutex_lock(&cpu->emulation_mutex);
    int n_cycles = emulateCpu8080Op(cpu, NULL);
    pthread_mutex_unlock(&cpu->emulation_mutex);
    return n_cycles;
}

void generateCpu8080Interrupt(Cpu8080* cpu, const uint8_t* interrupt_opcode)
{
    if (!cpu->interrupt_enabled) return;
    int op_bytes = disassembleCpu8080Op(cpu, interrupt_opcode);
    if (op_bytes == 0) exit(1);
    cpu->interrupt_enabled = 0;
    pthread_mutex_lock(&cpu->emulation_mutex);
    while (op_bytes > 0)
    {
        emulateCpu8080Op(cpu, interrupt_opcode);
        interrupt_opcode += op_bytes;
        op_bytes = disassembleCpu8080Op(cpu, interrupt_opcode);
    }
    pthread_mutex_unlock(&cpu->emulation_mutex);
    // It is assumed that the interrupt service routine will re-enable interrupts at the end of its execution
    // It is also assumed that ISR will restore Program Counter
    // TODO: check if previous statements are TRUE.
}

void runCpu8080(Cpu8080* cpu)
{
    while(1) emulateCpu8080(cpu);
}