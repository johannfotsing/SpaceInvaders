This project aims at emulating the Intel 8080 processor chip in order to play the once famous game SpaceInvaders.
This project is interesting for learning basics of computer architecture.
The project will be written mainly in C. I use the project as an opportunity to practice C language.
This work is inspired by the tutorial written by @emulator101 available on [Emulator101](http://www.emulator101.com) website.

See [8080](doc/8080.md) for information about the 8080 processor.

# CPU: a machine with a set of instructions

The 8080 CPU, as all processors, offers a set of instructions and operations that can be executed in a very tiny fraction of a second. They constitute the instructions set. The instructions are identified by an op code by the processor during program execution. But they are also written using a somehow intelligible code known as the *assembly* language. It marks the emancipation from the hardware paves the way into the software side of machine automation.

I copied the 8080 CPU instruction set from [here](http://www.emulator101.com), and listed them into *doc/instruction_set_8080.txt*. Each line maps an op code to an instruction's syntax and the operation description.

> ==0x80== 	_*ADD B*_	==1==	__*Z, S, P, CY, AC*__	==A <- A + B==

Elements of the line:
- *0x80*: the op code of the instruction (hexadecimal representation)
- *ADD B* : the assembly syntax of the instruction
- *1*: the number of memory bytes used by the instruction (basic scheme for parameter passing)
- *Z, S, P, CY, AC* the flags that are affected by the instruction's operation
- *A <- A + B*: describes the effect of the instruction's operation: which our 8080 CPU should emulate

==NOTE== : I will use and have probably done already the terms instruction and operation interchangeably.

## Program compilation and execution

The assembly language makes programming more of a human endeavour, an activity which very much resembles human communication.
Still, a processor is a machine that operates with binary digits, so it needs to be fed with zeros and ones.
It is the role of the compiler to translate the assembly language written by the programmer 

A processor executes a program in the following way.

- Load program into memory and point to the first byte of that memory space
- Start execution loop
- Read current program operation
- Execute program operation
- Increment the program counter of the number of memory bytes used by the instruction.
- Loop over

## The instruction set

CISC architecture ?

Instructions can be arranged into categories.

- Arithmetic and Logic instructions (ALU)
    - *ADDITION* instructions: ADD, ADC, ADI, ACI, DAD
    - *SUBSTRACTION* instructions: SUB, SBB, SUI, SBI
    - *BOOLEAN* instructions: ANA, ANI, XRA, XRI, ORA, ORI
    - *COMPARISON* instructions: CMP, CPI
    - *OTHER ARITHMETIC* instructions: DCR, INR, CDX, INX, RLC, RRC, RAL, RAR, STC,CMC, CMA

- Data exchange instructions (DEX)
    - *DATA MOVE* instructions: MOV, MVI, LXI, LDAX, LDA, STAX, STA
    - *STACK* instructions: PUSH, POP
    - *IO*: IN, OUT
    - *OTHERS*: 

- Binary control (BCH)
    - *RETURN* instructions
    - *JUMP* instructions
    - *CALL* instructions
    - *RESET* instructions

- Special instructions (SPC)
    - *INTERRUPTION* instructions: enable, disable, halt, nop
    - *Decimal Adjust Accumulator*: **************

## Implementation generation

We use some Python scripts to automatically generate the implementation C code of the instruction set.


# How to compile

The project's makefile defines three binary compilation targets: *diagnostic*, *spaceinvaders* and *spaceinvadersd*.
The targets depend on two libraries: the *SDL2* library used for arcade simulation, and the *pthread* library for parallelization.

## Target *diagnostic*

This target is used to test the *8080Cpu* implementation.

### Defines

## Target *spaceinvaders*

### Defines