import re
from _8080 import REGISTER_PAIRS

mov_instruction_parser = re.compile('MOV (?P<to_register>[A-EHLM]),[ ]*(?P<from_register>[A-EHLM]).*', re.A)
def generate_mov_code(op_label, syntax, num_bytes):
    """
    MOV instrutions are used to move bytes between CPU registers and memory boxes. 
    Syntax: MOV {DST_REG},{SRC_REG} | MOV M,{SRC_REG} | MOV {DST_REG},M
    M in the syntax refers to a memory location which address is given by HL register pair.
    """
    # Identify source and destination
    res = mov_instruction_parser.match(syntax)
    to_reg = res.group('to_register').lower()
    from_reg = res.group('from_register').lower()
    
    # Instruction for actual registers, not memory addresses 
    if (to_reg != 'M'.lower() and from_reg != 'M'.lower()):
        c_instruction = "case {}: cpu->{} = cpu->{}; cpu->pc += {}; break;\t\t\t\t// {}".format(op_label, to_reg, from_reg, num_bytes, syntax)
        return c_instruction
    # Destination register is actually a memory address
    elif (to_reg == 'M'.lower()):
        assert(from_reg != 'M'.lower()) # MOV M,M does not exist
        c_instruction = "case {}: writeCpu8080MemoryAtHL(cpu, cpu->{}); cpu->pc += {}; break;\t\t\t// {}".format(op_label, from_reg, num_bytes, syntax)
        return c_instruction
    # Source register is actually a memory address
    elif (from_reg == 'M'.lower()):
        assert(to_reg != 'M'.lower())   # MOV M,M does not exist
        c_instruction = "case {}: cpu->{} = readCpu8080MemoryAtHL(cpu); cpu->pc += {}; break;\t\t\t// {}".format(op_label, to_reg, num_bytes, syntax)
        return c_instruction
    # Error:
    else:
        assert(False)

mvi_instruction_parser = re.compile('MVI (?P<to_register>[A-EHLM]),[ ]*D8.*')
def generate_mvi_code(op_label, syntax, num_bytes):
    """
    MVI instructions are used to move immediate values of bytes into a register or memory.
    Syntax: MVI {DST_REG}, 0xXX | MVI M,0xXX
    """
    # Find source register
    to_register = mvi_instruction_parser.match(syntax).group('to_register').lower()

    # Actual register
    if to_register != 'M'.lower():
        c_instruction = "case {}: cpu->{} = opcode[1]; cpu->pc += {}; break;\t\t\t\t// {}".format(op_label, to_register, num_bytes, syntax)
        return c_instruction
    # Memory cell pointed to by HL registers pair
    else:
        c_instruction = "case {}: writeCpu8080MemoryAtHL(cpu, opcode[1]); cpu->pc += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)
        return c_instruction

def generate_lxi_code(op_label, syntax, num_bytes):
    """
    LXI instructions are used to load immediate word values into word registers.
    """
    register_high = re.match('LXI (?P<reg>[A-EHLSP]+),.*', syntax).group('reg').lower()
    # Stack pointer
    if register_high == 'SP'.lower():
        c_instruction = \
        "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tuint16_t h = opcode[2]; uint16_t l = opcode[1];\n"\
        + "\tcpu->sp = h << 8 | l;\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction
    else:
        register_low = REGISTER_PAIRS[register_high.upper()].lower()
        return "case {}: cpu->{} = opcode[2]; cpu->{} = opcode[1]; cpu->pc += {}; break;\t\t// {}".format(op_label, register_high, register_low, num_bytes, syntax)

def generate_ldax_code(op_label, syntax, num_bytes):
    """
    LDAX instructions load memory cell bytes into the accumulator register.
    """
    register_high = re.match('LDAX (?P<reg>[BD]).*', syntax).group('reg').lower()
    register_low = REGISTER_PAIRS[register_high.upper()].lower()
    c_instruction = \
    "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
    + "{\n"\
    + "\tcpu->a = readCpu8080Memory(cpu, ((uint16_t)cpu->{}) << 8 | ((uint16_t) cpu->{}));\n".format(register_high, register_low)\
    + "\tcpu->pc += {};\n".format(num_bytes)\
    + "\tbreak;\n"\
    + "}"
    return c_instruction

def generate_lda_code(op_label, syntax, num_bytes):
    """
    LDA instruction loads a byte into accumulator. The byte is read from memory at address given by immediate value.
    """
    return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
         + "{\n"\
         + "\tcpu->a = readCpu8080Memory(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]));\n"\
         + "\tcpu->pc += {};\n".format(num_bytes)\
         + "\tbreak;\n"\
         + "}"

def generate_stax_code(op_label, syntax, num_bytes):
    """
    STAX instructions load accumulator's byte into memory cell.
    """
    register_high = re.match('STAX (?P<reg>[BD]).*', syntax).group('reg').lower()
    register_low = REGISTER_PAIRS[register_high.upper()].lower()
    c_instruction = \
    "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
    + "{\n"\
    + "\twriteCpu8080Memory(cpu, ((uint16_t)cpu->{}) << 8 | ((uint16_t) cpu->{}), cpu->a);\n".format(register_high, register_low)\
    + "\tcpu->pc += {};\n".format(num_bytes)\
    + "\tbreak;\n"\
    + "}"
    return c_instruction

def generate_sta_code(op_label, syntax, num_bytes):
    """
    STA instruction stores accumulator's byte into memory. The memory cell address is given by immediate value.
    """
    return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
         + "{\n"\
         + "\twriteCpu8080Memory(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]), cpu->a);\n"\
         + "\tcpu->pc += {};\n".format(num_bytes)\
         + "\tbreak;\n"\
         + "}"

"""
STACK instructions
===================
"""

def generate_push_code(op_label, syntax, num_bytes):
    """
    PUSH instructions are used to push word register values on stack (for context switching).
    """
    register_high = re.match('PUSH (?P<reg>[BDHPSW]+).*', syntax).group('reg').lower()
    # If pushing accumulator and flags
    if register_high == 'PSW'.lower():
        return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
             + "{\n"\
             + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->a);\n"\
             + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, *((uint8_t*) &cpu->flags));\n"\
             + "\tcpu->pc += {};\n".format(num_bytes)\
             + "\tbreak;\n"\
             + "}"
    else:
        register_low = REGISTER_PAIRS[register_high.upper()].lower()
        return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
             + "{\n"\
             + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->{});\n".format(register_high)\
             + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, cpu->{});\n".format(register_low)\
             + "\tcpu->pc += {};\n".format(num_bytes)\
             + "\tbreak;\n"\
             + "}"

def generate_pop_code(op_label, syntax, num_bytes):
    """
    POP instructions are used to POP word register values from stack (for context restoration).
    """
    register_high = re.match('POP (?P<reg>[BDHPSW]+).*', syntax).group('reg').lower()
    # If poping accumulator and flags
    if register_high == 'PSW'.lower():
        return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
             + "{\n"\
             + "\t*((uint8_t*) &cpu->flags) = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
             + "\tcpu->a = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
             + "\tcpu->pc += {};\n".format(num_bytes)\
             + "\tbreak;\n"\
             + "}"
    else:
        register_low = REGISTER_PAIRS[register_high.upper()].lower()
        return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
             + "{\n"\
             + "\tcpu->{} = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n".format(register_low)\
             + "\tcpu->{} = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n".format(register_high)\
             + "\tcpu->pc += {};\n".format(num_bytes)\
             + "\tbreak;\n"\
             + "}"

"""
IO instrucions
===============
"""

def generate_in_code(op_label, syntax, num_bytes):
    """
    IN instruction loads an IO port byte into the accumulator.
    """
    return "case {}: cpu->a = cpu->in[opcode[1]].data; cpu->pc += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)

def generate_out_code(op_label, syntax, num_bytes):
    """
    OUT instruction writes the accumulator's value to an IO port.
    """
    return "case {}: cpu->out[opcode[1]].data = cpu->a; (*cpu->io_callbacks[opcode[1]])(cpu); cpu->pc += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)

"""
Others
============
"""

def generate_shld_code(op_label, syntax, num_bytes):
    """
    SHLD instruction is used to store HL word register to two contiguous memory cells which base address is provided as immediate value.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint16_t mem_offset = opcode[2];\n"\
                  + "\tmem_offset = (mem_offset << 8) | opcode[1];\n"\
                  + "\twriteCpu8080Memory(cpu, mem_offset, cpu->l);\n"\
                  + "\twriteCpu8080Memory(cpu, mem_offset + 1, cpu->h);\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_lhld_code(op_label, syntax, num_bytes):
    """
    LHLD instruction is used to load HL word register with values from two contiguous memory cells which base address is provided as immediate value.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint16_t mem_offset = opcode[2];\n"\
                  + "\tmem_offset = (mem_offset << 8) | opcode[1];\n"\
                  + "\tcpu->l = readCpu8080Memory(cpu, mem_offset);\n"\
                  + "\tcpu->h = readCpu8080Memory(cpu, mem_offset+1);\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_xthl_code(op_label, syntax, num_bytes):
    """
    XTHL instruction is used to exchange HL word register with memory cells pointed to by the stack pointer's value.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t l = cpu->l, h = cpu->h;\n"\
                  + "\tcpu->l = readCpu8080Memory(cpu, cpu->sp);\n"\
                  + "\tcpu->h = readCpu8080Memory(cpu, cpu->sp+1);\n"\
                  + "\twriteCpu8080Memory(cpu, cpu->sp, l);\n"\
                  + "\twriteCpu8080Memory(cpu, cpu->sp + 1, h);\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_pchl_code(op_label, syntax, num_bytes):
    """
    PCHL instruction loads HL register's word into the PC register.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc = cpu->h;\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | cpu->l;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_xchg_code(op_label, syntax, num_bytes):
    """
    XCHG instruction is used to exchange HL and DE register pairs.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t l = cpu->l, h = cpu->h;\n"\
                  + "\tcpu->l = cpu->e; cpu->e = l;\n"\
                  + "\tcpu->h = cpu->d; cpu->d = h;\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_sphl_code(op_label, syntax, num_bytes):
    """
    SPHL instruction loads HL register's word into the SP register.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->sp = cpu->h;\n"\
                  + "\tcpu->sp = (cpu->sp << 8) | cpu->l;\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction
