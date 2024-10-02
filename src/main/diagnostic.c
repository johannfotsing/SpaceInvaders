/**
 * 
 * @file cpu_8080_diagnostic.c
 * @author Johann FOTSING (johann.fotsing@outlook.com)
 * @brief The Cpu8080 implementation is diagnosed by emulating a program which results are known in advance (kind of a benchmarking program).
 * I use a diagnostic binary dowloaded from http://emulator101.com/files/cpudiag.bin. 
 * The diagnostic binary has been generated from a test program written by MicroCosm Associates for 8080/8085 CPUs.
 * For more information on the cpu diagnostic program and its compilation, please refer to http://www.emulator101.com/full-8080-emulation.html
 * 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "../include/cpu_8080/cpu_8080.h"

int main(int argc, char** argv)
{
    // Initialize CPU
    Cpu8080Config cfg = {8, 8, 64, 2e+6};
    Cpu8080* cpu = cpu8080_init(&cfg);

    // Load cpudiag binary into memory (with workarounds from http://www.emulator101.com/full-8080-emulation.html)
    size_t rom_size;
    uint8_t* rom = read_rom_from_file("./diag/cpudiag.bin", &rom_size);
    cpu8080_load_rom(cpu, 0x100, rom, rom_size);
    
    // Fix the first instruction to be JMP 0x100
    // Q: Why are we doing this again ?
    cpu8080_write_membyte(cpu, 0, 0xC3);
    cpu8080_write_membyte(cpu, 1, 0x00);
    cpu8080_write_membyte(cpu, 2, 0x01);
    
    // Fix the stack pointer from 0x6ad to 0x7ad
    // this 0x06 byte 112 in the code, which is
    // byte 112 + 0x100 = 368 in memory
    cpu8080_write_membyte(cpu, 368, 0x07);
    
    // Emulate CPU in a loop
    int nb_steps, nb_cycles;
    while(1)
    {
        fscanf(stdin, "%d", &nb_steps);
        for (; nb_steps > 0; --nb_steps)
            cpu8080_emulate(cpu, &nb_cycles);
    }
    
    cpu8080_free(cpu);
    
    return 0;
}