#ifndef CPU_8080_H
#define CPU_8080_H

#include <stdint.h>

typedef uint8_t bool_t;

/// 8080 structure declaration
struct _8080;
typedef struct _8080 Cpu8080;

/// 8080 CPU configuration
typedef struct _8080Config
{
    uint8_t in_ports;
    uint8_t out_ports;
    uint8_t memory_size_kb; // in kilobytes
    double frequency;       // in Hertz
} Cpu8080Config;

/// Constructor
Cpu8080* initCpu8080(const Cpu8080Config* conf);

/// Destructor
void freeCpu8080(Cpu8080*);

/**
 * @brief
 *      This function emulates the next operation to be executed in the CPU according to its program counter.
 * @param cpu    pointer to a 8080 CPU object.
 * @return int   the number of clock cycles consumed by the operation.
 */
int emulateCpu8080Op(Cpu8080* cpu);

/**
 * @brief 
 *      This function transforms the next operation code into human readable assembly code. 
 * @param cpu       pointer to a 8080 CPU object
 * @return int      the number of bytes used to encode the disassembled operation.
 */
int disassembleCpu8080Op(Cpu8080* cpu);

/**
 * @brief
 *      This function reads a specific bit on an IO port on a 8080 CPU
 * @param cpu           pointer to 8080 CPU object
 * @param port_number   output port where the bit is read
 * @param pin_number    pin number from where to read bit
 * @return uint8_t      0 or 1
 */
uint8_t readCpu8080IO(Cpu8080* cpu, int port_number, int pin_number);

/**
 * @brief
 *      This function writes a bit value on a specific pin of an IO port on a 8080 CPU
 * @param cpu           pointer to 8080 CPU object
 * @param port_number   input port where the bit is written
 * @param pin_number    index of the pin where the bit will be written
 * @param value         0 or 1
 */
void writeCpu8080IO(Cpu8080* cpu, int port_number, int pin_number, uint8_t value);

/**
 * @brief
 *      This function is used to read a byte from an output IO port on the CPU
 * @param cpu           pointer to a 8080 CPU
 * @param port_number   port number to read the byte from
 * @return uint8_t 
 */
uint8_t readCpu8080Port(Cpu8080* cpu, int port_number);

/**
 * @brief 
 *      This funciton is used to write a byte to an IO port of the 8080 CPU
 * @param cpu           pointer to a 8080 CPU
 * @param port_number   port number where to write the byte
 * @param value         byte value to write on the IO port
 */
void writeCpu8080Port(Cpu8080* cpu, int port_number, uint8_t value);

/// Output callback signature.
typedef void (*output_callback_t)();

/**
 * @brief Register a function to be called on output operation in an 8080 CPU
 * One can register as much callbacks as there are output ports.
 * The CPU is initialized with a fix number of IO ports.
 * For the moment, the validity of the port_number argument is not verified.
 * @param cpu       pointer to a 8080 CPU object
 * @param cb        a callback function executed when an OUT instruction is executed in an 8080 CPU
 */
void registerCpu8080OutputCallback(Cpu8080* cpu, output_callback_t cb, uint8_t port_number);

/**
 * @brief
 *      Write a byte into memory at a specific address
 * @param cpu       pointer to a 8080 cpu object
 * @param adr       memory address where to write the byte value
 * @param val       byte value to write into memory
 */
void writeCpu8080Memory(Cpu8080* cpu, uint16_t adr, uint8_t val);

/**
 * @brief
 *      Read a byte from memory
 * @param cpu       pointer to a 8080 cpu object
 * @param adr       memory address where to read byte value from
 * @return uint8_t  byte value read at memory address 
 */
uint8_t readCpu8080Memory(Cpu8080* cpu, uint16_t adr);

/**
 * @brief
 *      This function is used to load a ROM file into a 8080 CPU memory.
 * @param cpu               pointer to a 8080 CPU object
 * @param rom_path          filepath to the rom file to be loaded
 * @param memory_offset     memory address in CPU where the file write should start
 */
void loadCpu8080ROM(Cpu8080* cpu, const char* rom_path, uint16_t memory_offset);

#endif // CPU_8080_H
