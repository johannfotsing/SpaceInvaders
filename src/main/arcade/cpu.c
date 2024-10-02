#include "../../include/arcade/arcade.h"
#include "../../include/arcade/private/arcade.h"

void arcade_init_cpu(Arcade* arcade, const char* rom_folder_path)
{
    // CPU
    // 4 input ports
    // 8 output ports
    // 16kB of memory
    // 2 MHz frequency
    Cpu8080Config cpu_config = {4, 8, 16, 2.e+6};
    arcade->cpu = cpu8080_init(&cpu_config);
    
    /// Load SpaceInvaders ROM files
    size_t len = strlen(rom_folder_path);
    size_t copy_len = rom_folder_path[len-1] == '/' ? len - 1 : len;
    const char* first_rom_name = "/invaders.h";
    size_t rom_filename_len = strlen(first_rom_name);
    size_t file_path_len = copy_len + rom_filename_len;
    char* rom_file_path = (char*) malloc(file_path_len + 1);
    rom_file_path = strcpy(rom_file_path, rom_folder_path);
    rom_file_path = strcat(rom_file_path, first_rom_name);
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x0000);
    rom_file_path[file_path_len -1] = 'g';
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x0800);
    rom_file_path[file_path_len -1] = 'f';
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x1000);
    rom_file_path[file_path_len -1] = 'e';
    cpu8080_load_rom(arcade->cpu, rom_file_path, 0x1800);

    free(rom_file_path);

    // Load ROM into RAM
    memcpy(&arcade->cpu->memory[0x2000], &arcade->cpu->memory[0x0000], 0x2000);
    // Configure program counter for RAM in memory
    arcade->cpu->program_counter = 0x0000;
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
