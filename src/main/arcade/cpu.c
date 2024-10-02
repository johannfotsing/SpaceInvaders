#include "../../include/arcade/private/arcade.h"

void arcade_init_cpu(Arcade* arcade, const char* rom_file_path)
{
    // CPU
    // 4 input ports
    // 8 output ports
    // 16kB of memory
    // 2 MHz frequency
    Cpu8080Config cpu_config = {4, 8, 16, 2.e+6};
    arcade->cpu = cpu8080_init(&cpu_config);
    
    size_t rom_size;
    uint8_t* rom = read_rom_from_file(rom_file_path, &rom_size);
    
    // Load ROM into CPU memory
    cpu8080_load_rom(arcade->cpu, 0x0000, rom, rom_size);

    free(rom);
}

int arcade_emulate_cpu(void* a)
{
    Arcade* arcade = (Arcade*) a;
    
    while (arcade->running)
    {
        cpu8080_emulate(arcade->cpu, NULL);
    }

    return 0;
}
