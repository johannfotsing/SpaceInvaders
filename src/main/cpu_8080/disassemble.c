/*
* This file contains the implementation details related to the 8080 processor
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../include/cpu_8080/cpu_8080.h"
#include "../../include/cpu_8080/private/cpu_8080.h"


int cpu8080_disassemble_op(Cpu8080* cpu, const uint8_t* code)
{
    const uint8_t* opcode;
    if (code) opcode = code;
    else opcode = &cpu->memory[cpu->program_counter];
    int opbytes = 1;
    printf("%04x\t", cpu->program_counter);
    switch(*opcode)
    {
        case 0x00: printf("NOP"); break;
        case 0x01: printf("LXI      B, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;
        case 0x02: printf("STAX     B"); break;
        case 0x03: printf("INX      B"); break;
        case 0x04: printf("INR      B"); break;
        case 0x05: printf("DCR      B"); break;
        case 0x06: printf("MVI      B, #$%02x", opcode[1]); opbytes=2; break;
        case 0x07: printf("RLC      B"); break;
        case 0x08: printf("NOP"); break;
        case 0x09: printf("DAD      B"); break;
        case 0x0a: printf("LDAX     B"); break;
        case 0x0b: printf("DCX      B"); break;
        case 0x0c: printf("INR      C"); break;                                                   //     1     Z, S, P, AC     C <- C+1
        case 0x0d: printf("DCR      C"); break;                                                   //     1     Z, S, P, AC     C <-C-1
        case 0x0e: printf("MVI      C, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          C <- byte 2
        case 0x0f: printf("RRC"); break;                                                          //     1     CY     A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
        case 0x10: printf("NOP"); break;               
        case 0x11: printf("LXI      D, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;          // D16 3          D <- byte 3, E <- byte 2
        case 0x12: printf("STAX     D"); break;                                                   //     1          (DE) <- A
        case 0x13: printf("INX      D"); break;                                                   //     1          DE <- DE + 1
        case 0x14: printf("INR      D"); break;                                                   //     1     Z, S, P, AC     D <- D+1
        case 0x15: printf("DCR      D"); break;                                                   //     1     Z, S, P, AC     D <- D-1
        case 0x16: printf("MVI      D, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          D <- byte 2
        case 0x17: printf("RAL"); break;                                                          //     1     CY     A = A << 1; bit 0 = prev CY; CY = prev bit 7
        case 0x18: printf("NOP"); break;
        case 0x19: printf("DAD      D"); break;                                                   //     1     CY     HL = HL + DE
        case 0x1a: printf("LDAX     D"); break;                                                   //     1          A <- (DE)
        case 0x1b: printf("DCX      D"); break;                                                   //     1          DE = DE-1
        case 0x1c: printf("INR      E"); break;                                                   //     1     Z, S, P, AC     E <-E+1
        case 0x1d: printf("DCR      E"); break;                                                   //     1     Z, S, P, AC     E <- E-1
        case 0x1e: printf("MVI      E, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          E <- byte 2
        case 0x1f: printf("RAR"); break;                                                          //     1     CY     A = A >> 1; bit 7 = prev bit 7; CY = prev bit 0
        case 0x20: printf("RIM"); break;                                                          //     1          special
        case 0x21: printf("LXI      H, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;          //D16  3          H <- byte 3, L <- byte 2
        case 0x22: printf("SHLD     #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          (adr) <-L; (adr+1)<-H TODO: check
        case 0x23: printf("INX      H"); break;                                                   //     1          HL <- HL + 1
        case 0x24: printf("INR      H"); break;                                                   //     1     Z, S, P, AC     H <- H+1
        case 0x25: printf("DCR      H"); break;                                                   //     1     Z, S, P, AC     H <- H-1
        case 0x26: printf("MVI      H, #$%02x", opcode[1]); opbytes=2; break;                       // D8  2          L <- byte 2
        case 0x27: printf("DAA"); break;                                                          //     1          special
        case 0x28: printf("NOP"); break;
        case 0x29: printf("DAD      H"); break;                                                   //     1     CY     HL = HL + HI
        case 0x2a: printf("LHLD     #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          L <- (adr); H<-(adr+1)
        case 0x2b: printf("DCX      H"); break;                                                   //     1          HL = HL-1
        case 0x2c: printf("INR      L"); break;                                                   //     1     Z, S, P, AC     L <- L+1
        case 0x2d: printf("DCR      L"); break;                                                   //     1     Z, S, P, AC     L <- L-1
        case 0x2e: printf("MVI      L, #$%02x", opcode[1]); opbytes=2; break;                       //D8   2          L <- byte 2
        case 0x2f: printf("CMA"); break;                                                          //     1          A <- !A
        case 0x30: printf("SIM"); break;                                                          //     1          special
        case 0x31: printf("LXI      SP, #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;         //D16  3          SP.hi <- byte 3, SP.lo <- byte 2
        case 0x32: printf("STA      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          (adr) <- A
        case 0x33: printf("INX      SP"); break;                                                  //     1          SP = SP + 1
        case 0x34: printf("INR      M"); break;                                                   //     1     Z, S, P, AC     (HL) <- (HL)+1
        case 0x35: printf("DCR      M"); break;                                                   //     1     Z, S, P, AC     (HL) <- (HL)-1
        case 0x36: printf("MVI      M, #$%02x", opcode[1]); opbytes=2; break;                       //D8   2          (HL) <- byte 2
        case 0x37: printf("STC"); break;                                                          //     1     CY     CY = 1
        case 0x38: printf("NOP"); break;               
        case 0x39: printf("DAD      SP"); break;                                                  //     1     CY     HL = HL + SP
        case 0x3a: printf("LDA      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          A <- (adr)
        case 0x3b: printf("DCX      SP"); break;                                                  //     1          SP = SP-1
        case 0x3c: printf("INR      A"); break;                                                   //     1     Z, S, P, AC     A <- A+1
        case 0x3d: printf("DCR      A"); break;                                                   //     1     Z, S, P, AC     A <- A-1
        case 0x3e: printf("MVI      A, #$%02x", opcode[1]); opbytes=2; break;                       //D8   2          A <- byte 2
        case 0x3f: printf("CMC"); break;                                                          //     1          CY     CY=!CY
        case 0x40: printf("MOV      B, B"); break;                                                //     1          B <- B
        case 0x41: printf("MOV      B, C"); break;                                                //     1          B <- C
        case 0x42: printf("MOV      B, D"); break;                                                //     1          B <- D
        case 0x43: printf("MOV      B, E"); break;                                                //     1          B <- E
        case 0x44: printf("MOV      B, H"); break;                                                //     1          B <- H
        case 0x45: printf("MOV      B, L"); break;                                                //     1          B <- L
        case 0x46: printf("MOV      B, M"); break;                                                //     1          B <- (HL)
        case 0x47: printf("MOV      B, A"); break;                                                //     1          B <- A
        case 0x48: printf("MOV      C, B"); break;                                                //     1          C <- B
        case 0x49: printf("MOV      C, C"); break;                                                //     1          C <- C
        case 0x4a: printf("MOV      C, D"); break;                                                //     1          C <- D
        case 0x4b: printf("MOV      C, E"); break;                                                //     1          C <- E
        case 0x4c: printf("MOV      C, H"); break;                                                //     1          C <- H
        case 0x4d: printf("MOV      C, L"); break;                                                //     1          C <- L
        case 0x4e: printf("MOV      C, M"); break;                                                //     1          C <- (HL)
        case 0x4f: printf("MOV      C, A"); break;                                                //     1          C <- A
        case 0x50: printf("MOV      D, B"); break;                                                //     1          D <- B
        case 0x51: printf("MOV      D, C"); break;                                                //     1          D <- C
        case 0x52: printf("MOV      D, D"); break;                                                //     1          D <- D
        case 0x53: printf("MOV      D, E"); break;                                                //     1          D <- E
        case 0x54: printf("MOV      D, H"); break;                                                //     1          D <- H
        case 0x55: printf("MOV      D, L"); break;                                                //     1          D <- L
        case 0x56: printf("MOV      D, M"); break;                                                //     1          D <- (HL)
        case 0x57: printf("MOV      D, A"); break;                                                //     1          D <- A
        case 0x58: printf("MOV      E, B"); break;                                                //     1          E <- B
        case 0x59: printf("MOV      E, C"); break;                                                //     1          E <- C
        case 0x5a: printf("MOV      E, D"); break;                                                //     1          E <- D
        case 0x5b: printf("MOV      E, E"); break;                                                //     1          E <- E
        case 0x5c: printf("MOV      E, H"); break;                                                //     1          E <- H
        case 0x5d: printf("MOV      E, L"); break;                                                //     1          E <- L
        case 0x5e: printf("MOV      E, M"); break;                                                //     1          E <- (HL)
        case 0x5f: printf("MOV      E, A"); break;                                                //     1          E <- A
        case 0x60: printf("MOV      H, B"); break;                                                //     1          H <- B
        case 0x61: printf("MOV      H, C"); break;                                                //     1          H <- C
        case 0x62: printf("MOV      H, D"); break;                                                //     1          H <- D
        case 0x63: printf("MOV      H, E"); break;                                                //     1          H <- E
        case 0x64: printf("MOV      H, H"); break;                                                //     1          H <- H
        case 0x65: printf("MOV      H, L"); break;                                                //     1          H <- L
        case 0x66: printf("MOV      H, M"); break;                                                //     1          H <- (HL)
        case 0x67: printf("MOV      H, A"); break;                                                //     1          H <- A
        case 0x68: printf("MOV      L, B"); break;                                                //     1          L <- B
        case 0x69: printf("MOV      L, C"); break;                                                //     1          L <- C
        case 0x6a: printf("MOV      L, D"); break;                                                //     1          L <- D
        case 0x6b: printf("MOV      L, E"); break;                                                //     1          L <- E
        case 0x6c: printf("MOV      L, H"); break;                                                //     1          L <- H
        case 0x6d: printf("MOV      L, L"); break;                                                //     1          L <- L
        case 0x6e: printf("MOV      L, M"); break;                                                //     1          L <- (HL)
        case 0x6f: printf("MOV      L, A"); break;                                                //     1          L <- A
        case 0x70: printf("MOV      M, B"); break;                                                //     1          (HL) <- B
        case 0x71: printf("MOV      M, C"); break;                                                //     1          (HL) <- C
        case 0x72: printf("MOV      M, D"); break;                                                //     1          (HL) <- D
        case 0x73: printf("MOV      M, E"); break;                                                //     1          (HL) <- E
        case 0x74: printf("MOV      M, H"); break;                                                //     1          (HL) <- H
        case 0x75: printf("MOV      M, L"); break;                                                //     1          (HL) <- L
        case 0x76: printf("HLT"); break;                                                          //     1          special
        case 0x77: printf("MOV      M, A"); break;                                                //     1          (HL) <- C
        case 0x78: printf("MOV      A, B"); break;                                                //     1          A <- B
        case 0x79: printf("MOV      A, C"); break;                                                //     1          A <- C
        case 0x7a: printf("MOV      A, D"); break;                                                //     1          A <- D
        case 0x7b: printf("MOV      A, E"); break;                                                //     1          A <- E
        case 0x7c: printf("MOV      A, H"); break;                                                //     1          A <- H
        case 0x7d: printf("MOV      A, L"); break;                                                //     1          A <- L
        case 0x7e: printf("MOV      A, M"); break;                                                //     1          A <- (HL)
        case 0x7f: printf("MOV      A, A"); break;                                                //     1          A <- A
        case 0x80: printf("ADD      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + B
        case 0x81: printf("ADD      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + C
        case 0x82: printf("ADD      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + D
        case 0x83: printf("ADD      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + E
        case 0x84: printf("ADD      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + H
        case 0x85: printf("ADD      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + L
        case 0x86: printf("ADD      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + (HL)
        case 0x87: printf("ADD      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + A
        case 0x88: printf("ADC      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + B + CY
        case 0x89: printf("ADC      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + C + CY
        case 0x8a: printf("ADC      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + D + CY
        case 0x8b: printf("ADC      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + E + CY
        case 0x8c: printf("ADC      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + H + CY
        case 0x8d: printf("ADC      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + L + CY
        case 0x8e: printf("ADC      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + (HL) + CY
        case 0x8f: printf("ADC      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + A + CY
        case 0x90: printf("SUB      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - B
        case 0x91: printf("SUB      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - C
        case 0x92: printf("SUB      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + D
        case 0x93: printf("SUB      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - E
        case 0x94: printf("SUB      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + H
        case 0x95: printf("SUB      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - L
        case 0x96: printf("SUB      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A + (HL)
        case 0x97: printf("SUB      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - A
        case 0x98: printf("SBB      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - B - CY
        case 0x99: printf("SBB      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - C - CY
        case 0x9a: printf("SBB      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - D - CY
        case 0x9b: printf("SBB      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - E - CY
        case 0x9c: printf("SBB      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - H - CY
        case 0x9d: printf("SBB      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - L - CY
        case 0x9e: printf("SBB      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - (HL) - CY
        case 0x9f: printf("SBB      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A - A - CY
        case 0xa0: printf("ANA      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & B
        case 0xa1: printf("ANA      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & C
        case 0xa2: printf("ANA      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & D
        case 0xa3: printf("ANA      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & E
        case 0xa4: printf("ANA      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & H
        case 0xa5: printf("ANA      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & L
        case 0xa6: printf("ANA      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & (HL)
        case 0xa7: printf("ANA      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A & A
        case 0xa8: printf("XRA      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ B
        case 0xa9: printf("XRA      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ C
        case 0xaa: printf("XRA      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ D
        case 0xab: printf("XRA      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ E
        case 0xac: printf("XRA      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ H
        case 0xad: printf("XRA      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ L
        case 0xae: printf("XRA      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ (HL)
        case 0xaf: printf("XRA      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A ^ A
        case 0xb0: printf("ORA      B"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | B
        case 0xb1: printf("ORA      C"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | C
        case 0xb2: printf("ORA      D"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | D
        case 0xb3: printf("ORA      E"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | E
        case 0xb4: printf("ORA      H"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | H
        case 0xb5: printf("ORA      L"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | L
        case 0xb6: printf("ORA      M"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | (HL)
        case 0xb7: printf("ORA      A"); break;                                                   //     1     Z, S, P, CY, AC     A <- A | A
        case 0xb8: printf("CMP      B"); break;                                                   //     1     Z, S, P, CY, AC     A - B
        case 0xb9: printf("CMP      C"); break;                                                   //     1     Z, S, P, CY, AC     A - C
        case 0xba: printf("CMP      D"); break;                                                   //     1     Z, S, P, CY, AC     A - D
        case 0xbb: printf("CMP      E"); break;                                                   //     1     Z, S, P, CY, AC     A - E
        case 0xbc: printf("CMP      H"); break;                                                   //     1     Z, S, P, CY, AC     A - H
        case 0xbd: printf("CMP      L"); break;                                                   //     1     Z, S, P, CY, AC     A - L
        case 0xbe: printf("CMP      M"); break;                                                   //     1     Z, S, P, CY, AC     A - (HL)
        case 0xbf: printf("CMP      A"); break;                                                   //     1     Z, S, P, CY, AC     A - A
        case 0xc0: printf("RNZ"); break;                                                          //     1     if NZ, RET
        case 0xc1: printf("POP      B"); break;                                                   //     1     C <- (sp); B <- (sp+1); sp <- sp+2
        case 0xc2: printf("JNZ      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NZ, PC <- adr
        case 0xc3: printf("JMP      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          PC <= adr
        case 0xc4: printf("CNZ      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NZ, CALL adr
        case 0xc5: printf("PUSH     B"); break;                                                   //     1          (sp-2)<-C; (sp-1)<-B; sp <- sp - 2
        case 0xc6: printf("ADI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A + byte
        case 0xc7: printf("RST      0"); break;                                                   //     1          CALL $0
        case 0xc8: printf("RZ"); break;                                                           //     1          if Z, RET
        case 0xc9: printf("RET"); break;                                                          //     1          PC.lo <- (sp); PC.hi<-(sp+1); SP <- SP+2
        case 0xca: printf("JZ       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if Z, PC <- adr
        case 0xcb: printf("NOP"); break;
        case 0xcc: printf("CZ       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if Z, CALL adr
        case 0xcd: printf("CALL     #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          (SP-1)<-PC.hi;(SP-2)<-PC.lo;SP<-SP+2;PC=adr
        case 0xce: printf("ACI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A + data + CY
        case 0xcf: printf("RST      1"); break;                                                   //     1          CALL $8
        case 0xd0: printf("RNC"); break;                                                          //     1          if NCY, RET
        case 0xd1: printf("POP      D"); break;                                                   //     1          E <- (sp); D <- (sp+1); sp <- sp+2
        case 0xd2: printf("JNC      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NCY, PC<-adr
        case 0xd3: printf("OUT      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2          special
        case 0xd4: printf("CNC      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if NCY, CALL adr
        case 0xd5: printf("PUSH     D"); break;                                                   //     1          (sp-2)<-E; (sp-1)<-D; sp <- sp - 2
        case 0xd6: printf("SUI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A - data
        case 0xd7: printf("RST      2"); break;                                                   //     1          CALL $10
        case 0xd8: printf("RC"); break;                                                           //     1          if CY, RET
        case 0xd9: printf("NOP"); break;          
        case 0xda: printf("JC       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if CY, PC<-adr
        case 0xdb: printf("IN       #$%02x", opcode[1]); opbytes=2; break;                          //D8   2          special
        case 0xdc: printf("CC       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if CY, CALL adr
        case 0xdd: printf("NOP"); break;               
        case 0xde: printf("SBI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A - data - CY
        case 0xdf: printf("RST      3"); break;                                                   //     1          CALL $18
        case 0xe0: printf("RPO"); break;                                                          //     1          if PO, RET
        case 0xe1: printf("POP      H"); break;                                                   //     1          L <- (sp); H <- (sp+1); sp <- sp+2
        case 0xe2: printf("JPO      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PO, PC <- adr
        case 0xe3: printf("XTHL"); break;                                                         //     1          L <-> (SP); H <-> (SP+1)
        case 0xe4: printf("CPO      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PO, CALL adr
        case 0xe5: printf("PUSH     H"); break;                                                   //     1          (sp-2)<-L; (sp-1)<-H; sp <- sp - 2
        case 0xe6: printf("ANI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A & data
        case 0xe7: printf("RST      4"); break;                                                   //     1          CALL $20
        case 0xe8: printf("RPE"); break;                                                          //     1          if PE, RET
        case 0xe9: printf("PCHL"); break;                                                         //     1          PC.hi <- H; PC.lo <- L
        case 0xea: printf("JPE      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PE, PC <- adr
        case 0xeb: printf("XCHG"); break;                                                         //     1          H <-> D; L <-> E
        case 0xec: printf("CPE      #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if PE, CALL adr
        case 0xed: printf("NOP"); break;
        case 0xee: printf("XRI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A ^ data
        case 0xef: printf("RST      5"); break;                                                   //     1          CALL $28
        case 0xf0: printf("RP"); break;                                                           //     1          if P, RET
        case 0xf1: printf("POP      PSW"); break;                                                 //     1          flags <- (sp); A <- (sp+1); sp <- sp+2
        case 0xf2: printf("JP       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if P=1 PC <- adr
        case 0xf3: printf("DI"); break;                                                           //     1          special
        case 0xf4: printf("CP       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if P, PC <- adr
        case 0xf5: printf("PUSH     PSW"); break;                                                 //     1          (sp-2)<-flags; (sp-1)<-A; sp <- sp - 2
        case 0xf6: printf("ORI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A <- A | data
        case 0xf7: printf("RST      6"); break;                                                   //     1          CALL $30
        case 0xf8: printf("RM"); break;                                                           //     1          if M, RET
        case 0xf9: printf("SPHL"); break;                                                         //     1          SP=HL
        case 0xfa: printf("JM       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if M, PC <- adr
        case 0xfb: printf("EI"); break;                                                           //     1          special
        case 0xfc: printf("CM       #$%02x%02x", opcode[2], opcode[1]); opbytes=3; break;             //adr  3          if M, CALL adr
        case 0xfd: printf("NOP"); break;
        case 0xfe: printf("CPI      #$%02x", opcode[1]); opbytes=2; break;                          //D8   2     Z, S, P, CY, AC     A - data
        case 0xff: printf("RST      7"); break;                                                   //     1          CALL $38
        default: printf("Unknown code.\n"); return 0;
    }
    printf("\n");
    return opbytes;
}