/**
 * @file Cpu8080Diag.c
 * @author Johann FOTSING (johann.fotsing@outlook.com)
 * @brief Here is implemented the diagnostic of the Cpu8080 structure and associated function.
 *          I use a diagnostic binary dowloaded from http://emulator101.com/files/cpudiag.bin. 
 *          The diagnostic binary has been generated from a test program written by MicroCosm Associates for 8080/8085 CPUs.
 *          For more information on the cpu diagnostic program and its compilation, please refer to http://www.emulator101.com/full-8080-emulation.html
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "Cpu8080.h"

int main(int argc, char** argv)
{
    // Initialize CPU
    Cpu8080Config cfg = {8, 8, 64, 2e+6};
    Cpu8080* cpu = initCpu8080(&cfg);
    // Load cpudiag binary into memory (with workarounds from http://www.emulator101.com/full-8080-emulation.html)
    loadCpu8080ROM(cpu, "./rom/cpudiag.bin", 0x100);
    //Fix the first instruction to be JMP 0x100    
    writeCpu8080Memory(cpu, 0, 0xC3);
    writeCpu8080Memory(cpu, 1, 0x00);
    writeCpu8080Memory(cpu, 2, 0x01);
    //Fix the stack pointer from 0x6ad to 0x7ad
    // this 0x06 byte 112 in the code, which is
    // byte 112 + 0x100 = 368 in memory
    writeCpu8080Memory(cpu, 368, 0x07);
    // Emulate CPU in a loop
    while(1)
    {
        int nb_steps;
        scanf("%d", &nb_steps);
        for (int i=0; i<nb_steps; ++i)
            emulateCpu8080Op(cpu);
    }
    return 0;
}