/**
 * @file arcade.c
 * @author Johann Fotsing (johann.fotsing@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-06-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/// Map CPU 8080 input ports
#define INP0        0
#define INP1        1
#define INP2        2
#define SHFT_IN     3
/// Map CPU 8080 output ports
#define SHFT_AMNT   2
#define SOUND1      3
#define SHFT_DATA   4
#define SOUND2      5
#define WATCHDOG    6
/// Video
#define VIDEO_RAM_OFFSET 0x2400

#include <unistd.h>
#include <string.h>
#include "../../include/arcade.h"
#include "../../include/private/arcade.h"

Arcade* arcade_init(SDL_Window* w, SDL_Renderer* rd, SDL_Texture* tx, const char* rom_folder_path)
{
    Arcade* arc = (Arcade*) malloc(sizeof(Arcade));
    
    // Display
    arc->display.window = w;
    arc->display.renderer = rd;
    arc->display.texture = tx;
    Uint32 px_fmt;
    SDL_QueryTexture(tx, &px_fmt, NULL, &arc->display.width, &arc->display.height);
    arc->display.px_format = SDL_AllocFormat(px_fmt);
    
    // CPU
    Cpu8080Config cpuCfg = {4, 8, 16, 2.e+6};
    arc->cpu = cpu8080_init(&cpuCfg);
    /// Load SpaceInvaders ROM files
    size_t len = strlen(rom_folder_path);
    size_t copy_len = rom_folder_path[len-1] == '/' ? len - 1 : len;
    const char* first_rom_name = "/invaders.h";
    size_t rom_filename_len = strlen(first_rom_name);
    size_t file_path_len = copy_len + rom_filename_len;
    char* rom_file_path = (char*) malloc(file_path_len + 1);
    rom_file_path = strcpy(rom_file_path, rom_folder_path);
    rom_file_path = strcat(rom_file_path, first_rom_name);
    cpu8080_load_rom(arc->cpu, rom_file_path, 0);
    rom_file_path[file_path_len-1] = 'g';
    cpu8080_load_rom(arc->cpu, rom_file_path, 0x800);
    rom_file_path[file_path_len-1] = 'f';
    cpu8080_load_rom(arc->cpu, rom_file_path, 0x1000);
    rom_file_path[file_path_len-1] = 'e';
    cpu8080_load_rom(arc->cpu, rom_file_path, 0x1800);
    free(rom_file_path);

    // Shift register
    cpu8080_register_output_callback(arc->cpu, &on_shift_data_available, SHFT_AMNT);

    return arc;
}

void arcade_free(Arcade* a)
{
    cpu8080_free(a->cpu);
    SDL_FreeFormat(a->display.px_format);
    free(a);
}

void on_coin_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 0, 1);
}

void on_coin_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 0, 0);
}

void on_P2_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 1, 1);
}

void on_P2_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 1, 0);
}

void on_P1_start_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 2, 1);
}

void on_P1_start_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 2, 0);
}

void on_P1_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 4, 1);
}

void on_P1_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 4, 0);
}

void on_P1_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 5, 1);
}

void on_P1_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 5, 0);
}

void on_P1_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 6, 1);
}

void on_P1_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP1, 6, 0);
}

void on_P2_shoot_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 4, 1);
}

void on_P2_shoot_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 4, 0);
}

void on_P2_left_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 5, 1);
}

void on_P2_left_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 5, 0);
}

void on_P2_right_pressed(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 6, 1);
}

void on_P2_right_released(Arcade* a)
{
    cpu8080_write_io(a->cpu, INP2, 6, 0);
}

void on_shift_data_available(Cpu8080* cpu)
{
    static uint16_t shift_register = 0x0000;
    uint16_t shift_data = cpu8080_read_port(cpu, SHFT_DATA);
    shift_register = ((shift_register >> 8) & 0x00ff) | ((shift_data << 8) & 0xff00);
    uint8_t shift_offset = cpu8080_read_port(cpu, SHFT_AMNT) & 0b111; // offset is 3 bits long max
    uint8_t shift_result = ((shift_register << shift_offset) >> 8) & 0xff;
    cpu8080_write_port(cpu, SHFT_IN, shift_result);
}

void arcade_screen_update(Arcade* arcade)
{
    /* Copy bytes from index i, j in video memory 
       To index i_texture, j_texture in texture's pixels 
    */
    int pitch;
    uint8_t* texture_pixels;
    SDL_LockTexture(arcade->display.texture, NULL, (void**) &texture_pixels, &pitch);
    for (int byteIndex = 0; byteIndex < arcade->display.height/8; ++byteIndex)
    {
        uint8_t imageByte = cpu8080_read_membyte(arcade->cpu, VIDEO_RAM_OFFSET+byteIndex);
        for (int bitIndex=0; bitIndex<8; ++bitIndex)  // LSB first
        //for (int bitIndex=7; bitIndex>=0; --bitIndex)
        {
            int px_ram_idx = byteIndex * 8 + bitIndex;
            int i_ram = px_ram_idx / arcade->display.width;     // 0 - 223
            int j_ram = px_ram_idx % arcade->display.height;     // 0 - 255
            // Rotate 90deg left
            int j_txt = i_ram;
            int i_txt = (arcade->display.height - 1) - j_ram;
            // Turn bit pixel into color pixel
            // SDL_Color* px_color = &BLACK_COLOR;
            uint8_t pix_val = (imageByte & (1 << (bitIndex+1)))*255;
            texture_pixels[i_txt*arcade->display.width+j_txt] = SDL_MapRGBA(arcade->display.px_format, 255 - pix_val, 255 - pix_val, 0, 255);
        }
    }
    SDL_UnlockTexture(arcade->display.texture);
    SDL_RenderCopy(arcade->display.renderer, arcade->display.texture, NULL, NULL);
    SDL_RenderPresent(arcade->display.renderer);
}

void arcade_show(Arcade* arcade)
{
    while(1)
    {
        arcade_screen_update(arcade);
        usleep(1e+6/10);
    }
}

void start_video_interruption_thread(Arcade* arcade)
{
    __useconds_t half_display_period = 1e+6/120;
    uint8_t interrupt_number = 1;
    while (1)
    {
        usleep(half_display_period);
        emulate_space_invaders_interruption(arcade->cpu, interrupt_number);
        interrupt_number = 3 - interrupt_number;
    }
}

void emulate_space_invaders_interruption(Cpu8080* cpu, uint8_t interrupt_number)
{
    uint8_t interrupt_code[] = {0xc7, 0x00, 0x00};
    interrupt_code[1] = interrupt_number * 8;
    cpu8080_generate_interruption(cpu, interrupt_code);
}

void arcade_start(Arcade* arcade)
{
    pthread_create(&arcade->cpu_thread, NULL, cpu8080_run, arcade->cpu);
    pthread_create(&arcade->video_interrupt_thread, NULL, start_video_interruption_thread, arcade);
    pthread_create(&arcade->display_thread, NULL, arcade_show, arcade);
}