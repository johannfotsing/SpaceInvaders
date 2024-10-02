/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../../include/cpu_8080/private/cpu_8080.h"

/** Constructor & destructor **/
/******************************/

void output_unlinked(void* c, uint8_t data)
{

}

uint8_t input_unlinked(void* c)
{
    return 0;
}

Cpu8080* cpu8080_init(const Cpu8080Config* conf)
{
    // Allocate CPU memory space
    Cpu8080* c = (Cpu8080*) calloc(1, sizeof(Cpu8080));
    c->clock_frequency = conf->frequency;
    
    // Allocate memory
    size_t number_of_bytes = conf->memory_size_kb * 1024;
    c->memory = (uint8_t*) malloc(number_of_bytes);
    
    // Allocate IO ports
    c->in = (IOPort*) malloc(conf->in_ports * sizeof(IOPort));
    c->out = (IOPort*) malloc(conf->out_ports * sizeof(IOPort));
    
    // Allocate output callbacks array
    c->output_callbacks = (output_callback_t*) malloc(conf->out_ports * sizeof(output_callback_t));
    c->output_processors = (void**) malloc(sizeof(void*) * conf->out_ports);
    for (int i = 0; i < conf->out_ports; ++i)
    {
        c->output_processors[i] = c;
        c->output_callbacks[i] = &output_unlinked;
    }
    
    // Allocate output callbacks array
    c->input_callbacks = (input_callback_t*) malloc(conf->in_ports * sizeof(input_callback_t));
    c->input_processors = (void**) malloc(sizeof(void*) * conf->in_ports);
    for (int i = 0; i < conf->in_ports; ++i)
    {
        c->input_processors[i] = c;
        c->input_callbacks[i] = &input_unlinked;
    }
    
    // CPU state and interrupt enabling
    c->interrupt_enabled = false;
    c->stopped = false;

    return c;
}

void cpu8080_free(Cpu8080* cpu)
{
    free(cpu->output_callbacks);
    free(cpu->in);
    free(cpu->out);
    free(cpu->memory);
}


/** Memory operations **/
/***********************/

void cpu8080_load_rom(Cpu8080* cpu, uint16_t memory_offset, uint8_t* rom, size_t rom_size)
{
    memcpy(&cpu->memory[memory_offset], rom, rom_size);
}

void cpu8080_write_membyte(Cpu8080* cpu, uint16_t adr, uint8_t value)
{
#ifdef FOR_SPACE_INVADERS
    if (adr < 2 * 1024)   // attempt to write to ROM
    {
        printf("ERROR: attempt to write to ROM memory.");
        exit(1);
    }
#endif
    cpu->memory[adr] = value;
}

void cpu8080_read_membyte(Cpu8080* cpu, uint16_t adr, uint8_t* byte)
{
    *byte = cpu->memory[adr];
}

void cpu8080_write_HL_membyte(Cpu8080* cpu, uint8_t value)
{
    uint16_t adr = cpu->h;
    adr = adr << 8 | cpu->l;
    cpu8080_write_membyte(cpu, adr, value);
}

void cpu8080_read_HL_membyte(Cpu8080* cpu, uint8_t* byte)
{
    uint16_t adr = cpu->h;
    adr = adr << 8 | cpu->l;
    cpu8080_read_membyte(cpu, adr, byte);
}

uint8_t* read_rom_from_file(const char* filepath, size_t* rom_size)
{
    // Open ROM file
    FILE* rom_file = fopen(filepath, "rb");
    if (rom_file == NULL)
    {
        fprintf(stderr, "Failed to open file %s.", filepath);
        exit(1);
    }
    
    // Find ROM size
    fseek(rom_file, 0L, SEEK_END);
    *rom_size = ftell(rom_file);
    fseek(rom_file, 0L, SEEK_SET);
    
    // Copy file stream into CPU memory
    uint8_t* rom = malloc(*rom_size);
    fread(rom, 1, *rom_size, rom_file);
    
    // Close file
    if (fclose(rom_file) == EOF)
        exit(1);

    return rom;
}