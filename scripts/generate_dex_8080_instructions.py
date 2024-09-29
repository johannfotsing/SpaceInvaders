import re
from _8080 import REGISTER_PAIR_MAP

CPU8080_MOV_PARSER = re.compile('MOV (?P<dst_register>[A-EHLM]),[ ]*(?P<src_register>[A-EHLM]).*', re.A)


def generate_mov_code(op_label, syntax, num_bytes):
    """
    MOV instrutions are used to move bytes between CPU registers and memory locations.
    Syntax: MOV {DST_REG},{SRC_REG} | MOV M,{SRC_REG} | MOV {DST_REG},M
    M in the syntax refers to a memory location which address is given by HL register pair.
    """
    # Identify source and destination registers
    res = CPU8080_MOV_PARSER.match(syntax)
    dst_register = res.group('dst_register').lower()
    src_register = res.group('src_register').lower()
    
    # Instruction for actual registers, not memory addresses 
    if (dst_register != 'M'.lower() and src_register != 'M'.lower()):
        c_instruction = "case {}: cpu->{} = cpu->{}; cpu->program_counter += {}; break;\t\t\t\t// {}".format(op_label, dst_register, src_register, num_bytes, syntax)
        return c_instruction
    # Destination register is actually a memory address
    elif (dst_register == 'M'.lower()):
        assert(src_register != 'M'.lower()) # MOV M,M does not exist
        c_instruction = "case {}: cpu8080_write_HL_membyte(cpu, cpu->{}); cpu->program_counter += {}; break;\t\t\t// {}".format(op_label, src_register, num_bytes, syntax)
        return c_instruction
    # Source register is actually a memory address
    elif (src_register == 'M'.lower()):
        assert(dst_register != 'M'.lower())   # MOV M,M does not exist
        c_instruction = "case {}: cpu->{} = cpu8080_read_HL_membyte(cpu); cpu->program_counter += {}; break;\t\t\t// {}".format(op_label, dst_register, num_bytes, syntax)
        return c_instruction
    # Error:
    else:
        assert(False)


CPU8080_MVI_PARSER = re.compile('MVI (?P<dst_register>[A-EHLM]),[ ]*D8.*')


def generate_mvi_code(op_label, syntax, num_bytes):
    """
    MVI instructions are used to move immediate values of bytes into a register or into a memory location.
    Syntax: MVI {DST_REG}, 0xXX | MVI M, 0xXX
    """
    # Find source register
    dst_register = CPU8080_MVI_PARSER.match(syntax).group('dst_register').lower()

    # Actual register
    if dst_register != 'M'.lower():
        c_instruction = "case {}: cpu->{} = opcode[1]; cpu->program_counter += {}; break;\t\t\t\t// {}".format(op_label, dst_register, num_bytes, syntax)
        return c_instruction
    # Memory cell pointed to by HL registers pair
    else:
        c_instruction = "case {}: cpu8080_write_HL_membyte(cpu, opcode[1]); cpu->program_counter += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)
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
        + "\tcpu->stack_pointer = h << 8 | l;\n"\
        + "\tcpu->program_counter += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction
    else:
        register_low = REGISTER_PAIR_MAP[register_high.upper()].lower()
        return "case {}: cpu->{} = opcode[2]; cpu->{} = opcode[1]; cpu->program_counter += {}; break;\t\t// {}".format(op_label, register_high, register_low, num_bytes, syntax)


def generate_ldax_code(op_label, syntax, num_bytes):
    """
    LDAX instructions load memory cell bytes into the accumulator register.
    """
    register_high = re.match('LDAX (?P<register>[BD]).*', syntax).group('register').lower()
    register_low = REGISTER_PAIR_MAP[register_high.upper()].lower()
    c_instruction = \
    "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
    + "{\n"\
    + "\tcpu->a = cpu8080_read_membyte(cpu, ((uint16_t)cpu->{}) << 8 | ((uint16_t) cpu->{}));\n".format(register_high, register_low)\
    + "\tcpu->program_counter += {};\n".format(num_bytes)\
    + "\tbreak;\n"\
    + "}"
    return c_instruction


def generate_lda_code(op_label, syntax, num_bytes):
    """
    LDA instruction loads a memory byte into the accumulator byte register. The memory byte's address is given as immediate value.
    """
    return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
         + "{\n"\
         + "\tcpu->a = cpu8080_read_membyte(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]));\n"\
         + "\tcpu->program_counter += {};\n".format(num_bytes)\
         + "\tbreak;\n"\
         + "}"


def generate_stax_code(op_label, syntax, num_bytes):
    """
    STAX instruction loads accumulator's byte into memory cell which address is provided by a word register.
    """
    register_high = re.match('STAX (?P<reg>[BD]).*', syntax).group('reg').lower()
    register_low = REGISTER_PAIR_MAP[register_high.upper()].lower()
    c_instruction = \
    "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
    + "{\n"\
    + "\tcpu8080_write_membyte(cpu, ((uint16_t)cpu->{}) << 8 | ((uint16_t) cpu->{}), cpu->a);\n".format(register_high, register_low)\
    + "\tcpu->program_counter += {};\n".format(num_bytes)\
    + "\tbreak;\n"\
    + "}"
    return c_instruction


def generate_sta_code(op_label, syntax, num_bytes):
    """
    STA instruction stores accumulator's byte into memory. The memory cell address is given by immediate value.
    """
    return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
         + "{\n"\
         + "\tcpu8080_write_membyte(cpu, ((uint16_t) opcode[2]) << 8 | ((uint16_t) opcode[1]), cpu->a);\n"\
         + "\tcpu->program_counter += {};\n".format(num_bytes)\
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
             + "\tcpu->stack_pointer--; cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->a);\n"\
             + "\tcpu->stack_pointer--; cpu8080_write_membyte(cpu, cpu->stack_pointer, *((uint8_t*) &cpu->flags));\n"\
             + "\tcpu->program_counter += {};\n".format(num_bytes)\
             + "\tbreak;\n"\
             + "}"
    else:
        register_low = REGISTER_PAIR_MAP[register_high.upper()].lower()
        return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
             + "{\n"\
             + "\tcpu->stack_pointer--; cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->{});\n".format(register_high)\
             + "\tcpu->stack_pointer--; cpu8080_write_membyte(cpu, cpu->stack_pointer, cpu->{});\n".format(register_low)\
             + "\tcpu->program_counter += {};\n".format(num_bytes)\
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
             + "\t*((uint8_t*) &cpu->flags) = cpu8080_read_membyte(cpu, cpu->stack_pointer); cpu->stack_pointer++;\n"\
             + "\tcpu->a = cpu8080_read_membyte(cpu, cpu->stack_pointer); cpu->stack_pointer++;\n"\
             + "\tcpu->program_counter += {};\n".format(num_bytes)\
             + "\tbreak;\n"\
             + "}"
    else:
        register_low = REGISTER_PAIR_MAP[register_high.upper()].lower()
        return "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
             + "{\n"\
             + "\tcpu->{} = cpu8080_read_membyte(cpu, cpu->stack_pointer); cpu->stack_pointer++;\n".format(register_low)\
             + "\tcpu->{} = cpu8080_read_membyte(cpu, cpu->stack_pointer); cpu->stack_pointer++;\n".format(register_high)\
             + "\tcpu->program_counter += {};\n".format(num_bytes)\
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
    return "case {}: cpu->a = cpu->in[opcode[1]].data; cpu->program_counter += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)


def generate_out_code(op_label, syntax, num_bytes):
    """
    OUT instruction writes the accumulator's value to an IO port.
    """
    return "case {}: cpu->out[opcode[1]].data = cpu->a; (*cpu->io_callbacks[opcode[1]])(cpu); cpu->program_counter += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)


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
                  + "\tcpu8080_write_membyte(cpu, mem_offset, cpu->l);\n"\
                  + "\tcpu8080_write_membyte(cpu, mem_offset + 1, cpu->h);\n"\
                  + "\tcpu->program_counter += {};\n".format(num_bytes)\
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
                  + "\tcpu->l = cpu8080_read_membyte(cpu, mem_offset);\n"\
                  + "\tcpu->h = cpu8080_read_membyte(cpu, mem_offset+1);\n"\
                  + "\tcpu->program_counter += {};\n".format(num_bytes)\
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
                  + "\tcpu->l = cpu8080_read_membyte(cpu, cpu->stack_pointer);\n"\
                  + "\tcpu->h = cpu8080_read_membyte(cpu, cpu->stack_pointer+1);\n"\
                  + "\tcpu8080_write_membyte(cpu, cpu->stack_pointer, l);\n"\
                  + "\tcpu8080_write_membyte(cpu, cpu->stack_pointer + 1, h);\n"\
                  + "\tcpu->program_counter += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction


def generate_pchl_code(op_label, syntax, num_bytes):
    """
    PCHL instruction loads HL register's word into the PC register.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->program_counter = cpu->h;\n"\
                  + "\tcpu->program_counter = (cpu->program_counter << 8) | cpu->l;\n"\
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
                  + "\tcpu->program_counter += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction


def generate_sphl_code(op_label, syntax, num_bytes):
    """
    SPHL instruction loads HL register's word into the SP register.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->stack_pointer = cpu->h;\n"\
                  + "\tcpu->stack_pointer = (cpu->stack_pointer << 8) | cpu->l;\n"\
                  + "\tcpu->program_counter += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction
