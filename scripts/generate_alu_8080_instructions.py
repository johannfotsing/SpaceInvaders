import re
from _8080 import REGISTER_PAIRS

def generate_add_code(op_label, syntax, num_bytes):
    """
    ADD instructions are used to add a byte from a CPU register (or memory cell) to the accumulator register A
    """
    # Find addition source
    src_register = re.match('ADD (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080Add(cpu, cpu->a, cpu->{});\n".format(src_register)\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction
    else:
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080Add(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction

def generate_adc_code(op_label, syntax, num_bytes):
    """
    ADC instructions are used to add a byte from a CPU register (or memory cell) to the accumulator register A with the carry bit.
    """
    # Find addition source
    src_register = re.match('ADC (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080AddWithCarry(cpu, cpu->a, cpu->{});\n".format(src_register)\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction
    else:
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n\tuint16_t r = (uint16_t) cpu->a + (uint16_t)  + (uint16_t) cpu->flags.c;\n"\
        + "\tcpu->a = cpu8080AddWithCarry(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction

def generate_adi_code(op_label, syntax, num_bytes):
    """
    The ADI instruction adds an immediate value to the accumulator.
    """
    return "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080Add(cpu, cpu->a, opcode[1]);\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"

def generate_aci_code(op_label, syntax, num_bytes):
    """
    The ACI instruction adds an immediate value to the accumulator with the carry bit.
    """
    return "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080AddWithCarry(cpu, cpu->a, opcode[1]);\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"

def generate_sub_code(op_label, syntax, num_bytes):
    """
    SUB instructions are used to substract a byte in a CPU register (or memory cell) from the accumulator register A
    """
    # Find addition source
    src_register = re.match('SUB (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080Sub(cpu, cpu->a, cpu->{});\n".format(src_register)\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction
    else:
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080Sub(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction

def generate_sbb_code(op_label, syntax, num_bytes):
    """
    SBB instructions are used to substract a byte in a CPU register (or memory cell) from the accumulator register A with the carry bit borrowed.
    """
    # Find addition source
    src_register = re.match('SBB (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080SubWithBorrow(cpu, cpu->a, cpu->{});\n".format(src_register)\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction
    else:
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n\tuint16_t r = (uint16_t) cpu->a - (uint16_t) readCpu8080MemoryAtHL(cpu) - (uint16_t) cpu->flags.c;\n"\
        + "\tcpu->a = cpu8080SubWithBorrow(cpu, cpu->a, readCpu8080MemoryAtHL(cpu));\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"
        return c_instruction

def generate_sui_code(op_label, syntax, num_bytes):
    """
    The SUI instruction substracts an immediate value from the accumulator.
    """
    return "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080Sub(cpu, cpu->a, opcode[1]);\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"

def generate_sbi_code(op_label, syntax, num_bytes):
    """
    The ACI instruction substracts an immediate value from the accumulator with the carry bit.
    """
    return "case {}:\t\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tcpu->a = cpu8080SubWithBorrow(cpu, cpu->a, opcode[1]);\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        + "}"

def generate_ana_code(op_label, syntax, num_bytes):
    """
    ANA instructions are used for bitwise and between a byte from a CPU register (or memory cell) and the accumulator register A
    """
    # Find addition source
    src_register = re.match('ANA (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = "case {}: cpu->a &= cpu->{}; updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t\t// {}".format(op_label, src_register, num_bytes, syntax)
        return c_instruction
    # Memory cell pointed to by HL registers pair
    else:
        c_instruction = "case {}: cpu->a &= readCpu8080MemoryAtHL(cpu); updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)
        return c_instruction

def generate_ani_code(op_label, syntax, num_bytes):
    """
    ANI instruction are used for bitwise and between an immediate value (byte) and the accumulator register A
    """
    return "case {}: cpu->a &= opcode[1]; updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_xra_code(op_label, syntax, num_bytes):
    """
    XRA instructions are used for bitwise XOR between a byte from a CPU register (or memory cell) and the accumulator register A
    """
    # Find addition source
    src_register = re.match('XRA (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = "case {}: cpu->a ^= cpu->{}; updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t\t// {}".format(op_label, src_register, num_bytes, syntax)
        return c_instruction
    # Memory cell pointed to by HL registers pair
    else:
        c_instruction = "case {}: cpu->a ^= readCpu8080MemoryAtHL(cpu); updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)
        return c_instruction

def generate_xri_code(op_label, syntax, num_bytes):
    """
    XRI instruction are used for bitwise XOR between an immediate value (byte) and the accumulator register A
    """
    return "case {}: cpu->a ^= opcode[1]; updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_ora_code(op_label, syntax, num_bytes):
    """
    ORA instructions are used for bitwise OR between a byte from a CPU register (or memory cell) and the accumulator register A
    """
    # Find addition source
    src_register = re.match('ORA (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = "case {}: cpu->a |= cpu->{}; updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t\t// {}".format(op_label, src_register, num_bytes, syntax)
        return c_instruction
    # Memory cell pointed to by HL registers pair
    else:
        c_instruction = "case {}: cpu->a |= readCpu8080MemoryAtHL(cpu); updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t// {}".format(op_label, num_bytes, syntax)
        return c_instruction

def generate_ori_code(op_label, syntax, num_bytes):
    """
    ORI instruction are used for bitwise OR between an immediate value (byte) and the accumulator register A
    """
    return "case {}: cpu->a |= opcode[1]; updateCpu8080Flags(cpu, cpu->a); cpu->pc += {}; break;\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_cmp_code(op_label, syntax, num_bytes):
    """
    CMP instructions are used to compare a byte from a CPU register (or memory cell) to the accumulator's byte
    """
    # Find addition source
    src_register = re.match('CMP (?P<src_register>[A-EHLM]).*', syntax).group('src_register').lower()
    # Actual register
    if src_register != 'M'.lower():
        c_instruction = "case {}: cpu->flags.c = cpu->a < cpu->{}; cpu->flags.z = cpu->a == cpu->{}; cpu->pc += {}; break;\t\t// {}".format(op_label, src_register, src_register, num_bytes, syntax)
        return c_instruction
    # Memory cell pointed to by HL registers pair
    else:
        c_instruction = "case {}:\t\t\t\t// {}\n".format(op_label, syntax)\
                        + "{\n"\
                        + "\tuint8_t m = readCpu8080MemoryAtHL(cpu);\n"\
                        + "\tcpu->flags.c = cpu->a < m;\n"\
                        + "\tcpu->flags.z = cpu->a == m;\n"\
                        + "\tcpu->pc += {};\n".format(num_bytes)\
                        + "\tbreak;\n"\
                        + "}"
        return c_instruction

def generate_cpi_code(op_label, syntax, num_bytes):
    """
    CPI instruction compares an immediate value (byte) to the accumulator's byte
    """
    return "case {}: cpu->flags.c = cpu->a < opcode[1]; cpu->flags.z = cpu->a == opcode[1]; cpu->pc += {}; break;\t// {}".format(op_label, num_bytes, syntax)

def generate_dcr_code(op_label, syntax, num_bytes):
    """
    DCR instructions decrement the value stored into a CPU register or a memory cell.
    Syntax: DCR {REG}
    """
    register = re.match('DCR (?P<reg>[A-EHLM]).*', syntax).group('reg').lower()
    # Actual register
    if register != 'M'.lower():
        c_instruction = "case {}: cpu->{}--; updateCpu8080Flags(cpu, cpu->{}); cpu->pc += {}; break;\t\t\t// {}".format(op_label, register, register, num_bytes, syntax)
        return c_instruction
    # Memory cell
    else:
        c_instruction = "case {}: writeCpu8080MemoryAtHL(cpu, readCpu8080MemoryAtHL(cpu) - 1); updateCpu8080Flags(cpu, readCpu8080MemoryAtHL(cpu)); cpu->pc += {}; break;\t// {}".format(op_label, num_bytes, syntax)
        return c_instruction

def generate_inr_code(op_label, syntax, num_bytes):
    """
    INR instructions increment the value stored into a CPU register or a memory cell.
    Syntax: INR {REG}
    """
    register = re.match('INR (?P<reg>[A-EHLM]).*', syntax).group('reg').lower()
    # Actual register
    if register != 'M'.lower():
        c_instruction = "case {}: cpu->{}++; updateCpu8080Flags(cpu, cpu->{}); cpu->pc += {}; break;\t\t\t// {}".format(op_label, register, register, num_bytes, syntax)
        return c_instruction
    # Memory cell
    else:
        c_instruction = "case {}: writeCpu8080MemoryAtHL(cpu, readCpu8080MemoryAtHL(cpu) + 1); updateCpu8080Flags(cpu, readCpu8080MemoryAtHL(cpu)); cpu->pc += {}; break;\t// {}".format(op_label, num_bytes, syntax)
        return c_instruction

def generate_stc_code(op_label, syntax, num_bytes):
    """
    STC instruction sets the carry bit.
    """
    return "case {}: cpu->flags.c = 0b1; cpu->pc += {}; break;\t\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_cmc_code(op_label, syntax, num_bytes):
    """
    CMC instruction complemets the carry bit.
    """
    return "case {}: cpu->flags.c = ~cpu->flags.c; cpu->pc += {}; break;\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_cma_code(op_label, syntax, num_bytes):
    """
    CMA instruction complemets the accumulator byte.
    """
    return "case {}: cpu->a = ~cpu->a; cpu->pc += {}; break;\t\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_dad_code(op_label, syntax, num_bytes):
    """
    DAD instructions are used to ADD word registers' values to HL word register. The HL register is the accumulator for word operations.
    """
    register_high = re.match('DAD (?P<reg>[A-EHLSP]+).*', syntax).group('reg').lower()
    # Stack pointer is already 16bits
    if register_high == 'SP'.lower():
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tuint16_t h = (uint16_t) cpu->h;\n"\
        + "\tuint16_t l = (uint16_t) cpu->l;\n"\
        + "\tuint16_t hl = h << 8 | l;\n"\
        + "\tuint32_t r = (uint32_t) hl + (uint32_t) cpu->sp;\n"\
        + "\tcpu->flags.c = (r > 0xffff);\n"\
        + "\tcpu->l = (uint8_t) r & 0xff;\n"\
        + "\tcpu->h = (uint8_t) r >> 8 & 0xff;\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        +"}"
        return c_instruction
    else:
        register_low = REGISTER_PAIRS[register_high.upper()].lower()
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tuint16_t h = (uint16_t) cpu->h;\n"\
        + "\tuint16_t l = (uint16_t) cpu->l;\n"\
        + "\tuint16_t hl = h << 8 | l;\n"\
        + "\tuint16_t rh = (uint16_t) cpu->{};\n".format(register_high)\
        + "\tuint16_t rl = (uint16_t) cpu->{};\n".format(register_low)\
        + "\tuint16_t rhl = rh << 8 | rl;\n"\
        + "\tuint32_t r = (uint32_t) hl + (uint32_t) rhl;\n"\
        + "\tcpu->flags.c = (r > 0xffff);\n"\
        + "\tcpu->l = (uint8_t) r & 0xff;\n"\
        + "\tcpu->h = (uint8_t) (r >> 8) & 0xff;\n"\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        +"}"
        return c_instruction

def generate_inx_code(op_label, syntax, num_bytes):
    """
    INX instructions increment word registers.
    """
    register_high = re.match('INX (?P<reg>[A-EHLSP]+).*', syntax).group('reg').lower()
    if register_high == 'SP'.lower():
        return "case {}: cpu->sp++; cpu->pc += {}; break;\t\t\t\t// {}".format(op_label, num_bytes, syntax)
    else:
        register_low = REGISTER_PAIRS[register_high.upper()].lower()
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tuint16_t w = (uint16_t) cpu->{};\n".format(register_high)\
        + "\tw = (w << 8) | cpu->{}; w++;\n".format(register_low)\
        + "\tcpu->{} = w & 0xff;\n".format(register_low)\
        + "\tcpu->{} = (w >> 8) & 0xff;\n".format(register_high)\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        +"}"
        return c_instruction

def generate_dcx_code(op_label, syntax, num_bytes):
    """
    DCX instructions decrement word registers.
    """
    register_high = re.match('DCX (?P<reg>[A-EHLSP]+).*', syntax).group('reg').lower()
    if register_high == 'SP'.lower():
        return "case {}: cpu->sp--; cpu->pc += {}; break;\t\t\t\t// {}".format(op_label, num_bytes, syntax)
    else:
        register_low = REGISTER_PAIRS[register_high.upper()].lower()
        c_instruction = \
        "case {}:\t\t\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
        + "{\n"\
        + "\tuint16_t w = (uint16_t) cpu->{};\n".format(register_high)\
        + "\tw = (w << 8) | cpu->{}; w--;\n".format(register_low)\
        + "\tcpu->{} = w & 0xff;\n".format(register_low)\
        + "\tcpu->{} = (w >> 8) & 0xff;\n".format(register_high)\
        + "\tcpu->pc += {};\n".format(num_bytes)\
        + "\tbreak;\n"\
        +"}"
        return c_instruction

def generate_rlc_code(op_label, syntax, num_bytes):
    """
    RLC instruction is used to perform a circular rotation of the accumulator byte in the left direction.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t prev_bit_7 = (cpu->a >> 7) & 0x01;\n"\
                  + "\tcpu->a = cpu->a << 1;\n"\
                  + "\tcpu->a = cpu->a | prev_bit_7;\n"\
                  + "\tcpu->flags.c = prev_bit_7;\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rrc_code(op_label, syntax, num_bytes):
    """
    RRC instruction is used to perform a circular rotation of the accumulator byte in the right direction.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t prev_bit_0 = cpu->a & 0x01;\n"\
                  + "\tcpu->a = cpu->a >> 1;\n"\
                  + "\tcpu->a = cpu->a | (prev_bit_0 << 7);\n"\
                  + "\tcpu->flags.c = prev_bit_0;\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_ral_code(op_label, syntax, num_bytes):
    """
    RAR instruction is used to shift the accumulator one bit to the left.
    The carry becomes the low order bit and the high order bit goes to the carry.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t prev_bit_7 = (cpu->a >> 7) & 0x01;\n"\
                  + "\tcpu->a = cpu->a << 1;\n"\
                  + "\tcpu->a = cpu->a | cpu->flags.c;\n"\
                  + "\tcpu->flags.c = prev_bit_7;\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rar_code(op_label, syntax, num_bytes):
    """
    RAR instruction is used to shift the accumulator one bit to the right.
    The carry becomes the high order bit and the low order bit goes to the carry.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t prev_bit_0 = cpu->a & 0x01;\n"\
                  + "\tcpu->a = cpu->a >> 1;\n"\
                  + "\tcpu->a = cpu->a | (cpu->flags.c << 7);\n"\
                  + "\tcpu->flags.c = prev_bit_0;\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction
