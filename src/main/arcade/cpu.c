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

    Uint32 emulation_period_ms = 2;
    Uint64 nb_cycles_per_ms = 2e+6 / 1e+3;
    Uint64 last_emulation_time = SDL_GetTicks64();

    // 
    Uint64 nb_cycles_emulated = 0;
    Uint64 max_cycles_to_emulate;
    int nb_op_cycles;
    
    while (arcade->running)
    {
        if (SDL_GetTicks64() - last_emulation_time > emulation_period_ms)
        {
            max_cycles_to_emulate = (SDL_GetTicks64() - last_emulation_time) * nb_cycles_per_ms;
            max_cycles_to_emulate = max_cycles_to_emulate > 4000 ? 4000 * 1.5: max_cycles_to_emulate;
            last_emulation_time = SDL_GetTicks64();
            nb_cycles_emulated = 0;
            while (nb_cycles_emulated < max_cycles_to_emulate)
            {
                cpu8080_emulate(arcade->cpu, &nb_op_cycles);
                nb_cycles_emulated += nb_op_cycles;
            }
        }
        SDL_Delay(1);
    }

    return 0;
}
