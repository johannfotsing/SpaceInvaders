/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080.h"
#include "../../include/private/cpu_8080.h"

/** Constructor & destructor **/
/******************************/

Cpu8080* cpu8080_init(const Cpu8080Config* conf)
{
    // Allocate CPU memory space
    Cpu8080* c = (Cpu8080*) calloc(1, sizeof(Cpu8080));
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
    c->stack_pointer = 0x2300;
}

void cpu8080_free(Cpu8080* cpu)
{
    free(cpu->io_callbacks);
    free(cpu->in);
    free(cpu->out);
    free(cpu->memory);
}


/** Memory operations **/
/***********************/

void cpu8080_load_rom(Cpu8080* cpu, const char* rom_path, uint16_t memory_offset)
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

void cpu8080_write_membyte(Cpu8080* cpu, uint16_t adr, uint8_t value)
{
#ifdef FOR_SPACE_INVADERS
    if (adr < 2*1024)   // attempt to write to ROM
    {
        printf("ERROR: attempt to write to ROM memory.");
        exit(1);
    }
#endif
    cpu->memory[adr] = value;
}

uint8_t cpu8080_read_membyte(Cpu8080* cpu, uint16_t adr)
{
    return cpu->memory[adr];
}

void cpu8080_write_HL_membyte(Cpu8080* cpu, uint8_t value)
{
    uint16_t adr = cpu->h;
    adr = adr << 8 | cpu->l;
    cpu8080_write_membyte(cpu, adr, value);
}

uint8_t cpu8080_read_HL_membyte(Cpu8080* cpu)
{
    uint16_t adr = cpu->h;
    adr = adr << 8 | cpu->l;
    return cpu8080_read_membyte(cpu, adr);
}