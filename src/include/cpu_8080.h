#ifndef CPU_8080_H
#define CPU_8080_H

#include <stdint.h>
#include <stdbool.h>

/// 8080 structure declaration
struct _8080;
typedef struct _8080 Cpu8080;

/// 8080 CPU configuration
typedef struct _8080Config
{
    uint8_t in_ports;       ///< Number of input ports
    uint8_t out_ports;      ///< Number of output ports
    uint8_t memory_size_kb; ///< CPU memory size in kilobytes
    double frequency;       ///< Operation frequency in Hertz
} Cpu8080Config;

/// Constructor
Cpu8080* cpu8080_init(const Cpu8080Config* conf);

/// Destructor
void cpu8080_free(Cpu8080*);

/**
 * @brief 
 * @param cpu                   pointer to a 8080 CPU object
 */
void cpu8080_emulate(Cpu8080* cpu, int* nb_cycles);

/**
 * @brief 
 * @param cpu                   pointer to a 8080 CPU object
 */
void cpu8080_run(Cpu8080* cpu);

/**
 * @brief
 *      Write a byte into memory at a specific address (ROM ? RAM ?)
 * @param cpu       pointer to a 8080 cpu object
 * @param adr       memory address where to write the byte value
 * @param val       byte value to write into memory
 */
void cpu8080_write_membyte(Cpu8080* cpu, uint16_t adr, uint8_t val);

/**
 * @brief
 *      Read a byte from memory
 * @param cpu       pointer to a 8080 cpu object
 * @param adr       memory address where to read byte value from
 * @param byte      reference to byte where value at memory address is read
 */
// TODO: write read byte into a reference parameter
void cpu8080_read_membyte(Cpu8080* cpu, uint16_t adr, uint8_t* byte);

/**
 * @brief
 *      This function is used to load a ROM file into a 8080 CPU memory.
 * @param cpu               pointer to a 8080 CPU object
 * @param rom_path          filepath to the rom file to be loaded
 * @param memory_offset     memory address in CPU where the file write should start
 */
void cpu8080_load_rom(Cpu8080* cpu, const char* rom_path, uint16_t memory_offset);

/**
 * @brief 
 *      When an interruption is generated, the normal flow of execution is interrupted. 
 * The interrupt source puts instruction bytes on the data bus that the CPU reads and executes.
 * Generally, the interrupt instruction is used to call a predefined function in ROM using RST instructions; 
 * this way the program counter is pushed on stack and the program can resume after the interrupt routine. 
 * @param cpu                   pointer to a 8080 CPU object
 * @param isr_index             index of the interrupt subroutine to execute (one of the RSTs)
 */
void cpu8080_generate_interruption(Cpu8080* cpu, const uint8_t isr_index);

/**
 * @brief
 *      This function reads a specific bit on an IO port on a 8080 CPU
 * @param cpu           pointer to 8080 CPU object
 * @param port_number   output port where the bit is read
 * @param pin_number    pin number from where to read bit
 * @param byte          reference to the byte where io value will be read
 * @return uint8_t      0 or 1
 */
void cpu8080_read_io(Cpu8080* cpu, int port_number, int pin_number, uint8_t* byte);

/**
 * @brief
 *      This function writes a bit value on a specific pin of an IO port on a 8080 CPU
 * @param cpu           pointer to 8080 CPU object
 * @param port_number   input port where the bit is written
 * @param pin_number    index of the pin where the bit will be written
 * @param value         0 or 1
 */
void cpu8080_write_io(Cpu8080* cpu, int port_number, int pin_number, uint8_t value);

/**
 * @brief
 *      This function is used to read a byte from an output IO port on the CPU
 * @param cpu           pointer to a 8080 CPU
 * @param port_number   port number to read the byte from
 * @param byte          reference to the byte where the cpu port will be read
 */
void cpu8080_read_port(Cpu8080* cpu, int port_number, uint8_t* byte);

/**
 * @brief 
 *      This funciton is used to write a byte to an IO port of the 8080 CPU
 * @param cpu           pointer to a 8080 CPU
 * @param port_number   port number where to write the byte
 * @param value         byte value to write on the IO port
 */
void cpu8080_write_port(Cpu8080* cpu, int port_number, uint8_t value);

/// Output callback signature.
typedef void (*output_callback_t)(Cpu8080*);

/**
 * @brief Register a function to be called on output operation in an 8080 CPU
 * One can register as much callbacks as there are output ports.
 * The CPU is initialized with a fix number of IO ports.
 * For the moment, the validity of the port_number argument is not verified.
 * @param cpu       pointer to a 8080 CPU object
 * @param cb        a callback function executed when an OUT instruction is executed in an 8080 CPU
 */
void cpu8080_register_output_callback(Cpu8080* cpu, output_callback_t cb, uint8_t port_number);

#endif // CPU_8080_H
