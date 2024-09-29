#ifndef CPU_8080_PRIVATE_H
#define CPU_8080_PRIVATE_H

#include <pthread.h>
#include "../cpu_8080.h"

/// This structure describes the status register of a 8080 processor (ALU ?).
typedef struct _8080Status
{
    uint8_t s: 1;
    uint8_t z: 1;
    uint8_t _: 1;       // padding
    uint8_t ac: 1;
    uint8_t __: 1;      // padding
    uint8_t p: 1;
    uint8_t ___: 1;     // padding
    uint8_t c: 1;
} Status;

/// This structure describes an 8080 IO port
typedef union _8080Port
{
    struct 
    {
        uint8_t bit_0: 1;
        uint8_t bit_1: 1;
        uint8_t bit_2: 1;
        uint8_t bit_3: 1;
        uint8_t bit_4: 1;
        uint8_t bit_5: 1;
        uint8_t bit_6: 1;
        uint8_t bit_7: 1;
    } bits;
    uint8_t data;
} IOPort;

/// This structure describes a 8080 processor.
struct _8080
{
    /// Main Registers
    uint8_t a;      // accumulator
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    
    /// Index Registers
    // Program counter
    uint16_t program_counter;
    // Stack pointer
    uint16_t stack_pointer;
    
    /// Status Register
    Status flags;
    
    /// Memory
    uint8_t* memory;
    
    /// Interruption management
    bool_t interrupt_enabled;
    
    /// A flag to stop the CPU
    bool_t stopped;
    
    /// IO
    IOPort* in;
    IOPort* out;
    output_callback_t* io_callbacks;

    // Clock
    double nsec_per_cycle;

    // A mutex to handle emulation from main thread and interrupt threads
    // TODO: remove this from here !?
    pthread_mutex_t emulation_mutex;
};

/**
 * @brief
 *      
 * @param cpu       pointer to a 8080 cpu object
 * @param code      bytecode to emulate
 */
int cpu8080_emulate_op(Cpu8080* cpu, const uint8_t* code);

/**
 * @brief
 *      
 * @param cpu       pointer to a 8080 cpu object
 * @param code      bytecode to disassemble
 */
int cpu8080_disassemble_op(Cpu8080* cpu, const uint8_t* code);

/**
 * @brief
 *      Write a byte into memory address pointed by HL register pair
 * @param cpu       pointer to a 8080 cpu object
 * @param value     byte value to store into memory
 */
void cpu8080_write_HL_membyte(Cpu8080* cpu, uint8_t value);

/**
 * @brief
 *      Read a byte from memory at address pointed by HL register pair
 * @param cpu       pointer to a 8080 cpu object
 * @return uint8_t  byte read from memory
 */
// TODO: pass register value into a reference parameter
uint8_t cpu8080_read_HL_membyte(Cpu8080* cpu);

/**
 * @brief 
 *      This function updates the status register according to the result from previous ALU operation.
 * @param cpu       pointer to a 8080 CPU object
 * @param r         result from last ALU operation
 */
void cpu8080_update_flags(Cpu8080* cpu, uint16_t r);

/**
 * @brief 
 *      This function prints CPU registers and flags
 * @param cpu   pointer to 8080 CPU object
 */
void cpu8080_print_state(Cpu8080* cpu);


/** Helper functions for arithmetic operations **/
/************************************************/
/************************************************/
/************************************************/
/************************************************/


void cpu8080_update_flags(Cpu8080* cpu, uint16_t r);

uint8_t cpu8080_ALU_add(Cpu8080* cpu, uint8_t a, uint8_t b);

uint8_t cpu8080_ALU_add_with_carry(Cpu8080* cpu, uint8_t a, uint8_t b);

uint8_t cpu8080_ALU_twos_complement(uint8_t a);

uint8_t cpu8080_ALU_substract(Cpu8080* cpu, uint8_t a, uint8_t b);

uint8_t cpu8080_ALU_substract_with_borrow(Cpu8080* cpu, uint8_t a, uint8_t b);

// TODO: find out what that operation does and implement
void cpu8080DAA(Cpu8080* cpu);

#endif // CPU_8080_PRIVATE_H