import re

"""
RETURN instructions
====================
"""

def generate_ret_code(op_label, syntax, num_bytes):
    """
    RET instruction is used to return from a function call.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rz_code(op_label, syntax, num_bytes):
    """
    RZ instruction is used to return from a function call if Zero flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.z) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rnz_code(op_label, syntax, num_bytes):
    """
    RNZ instruction is used to return from a function call if Zero flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.z) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rc_code(op_label, syntax, num_bytes):
    """
    RC instruction is used to return from a function call if Carry flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.c) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rnc_code(op_label, syntax, num_bytes):
    """
    RNC instruction is used to return from a function call if Carry flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.c) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rpe_code(op_label, syntax, num_bytes):
    """
    RPE instruction is used to return from a function call if Parity flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.p) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rpo_code(op_label, syntax, num_bytes):
    """
    RPO instruction is used to return from a function call if Parity flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.p) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rp_code(op_label, syntax, num_bytes):
    """
    RP instruction is used to return from a function call if Sign flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.s) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rm_code(op_label, syntax, num_bytes):
    """
    RM instruction is used to return from a function call if Sign flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.s) break;"\
                  + "\tuint16_t pcl = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tuint16_t pch = readCpu8080Memory(cpu, cpu->sp); cpu->sp++;\n"\
                  + "\tcpu->pc = (pch << 8) | pcl;\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

"""
JUMP instructions
==================
"""

def generate_jmp_code(op_label, syntax, num_bytes):
    """
    JMP instruction is used to jump to a specific instruction in the program.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jz_code(op_label, syntax, num_bytes):
    """
    JZ instruction is used to jump to a specific instruction in the program if the Zero flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.z) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jnz_code(op_label, syntax, num_bytes):
    """
    JNZ instruction is used to jump to a specific instruction in the program if the Zero flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.z) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jc_code(op_label, syntax, num_bytes):
    """
    JC instruction is used to jump to a specific instruction in the program if the Carry flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.c) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jnc_code(op_label, syntax, num_bytes):
    """
    JNC instruction is used to jump to a specific instruction in the program if the Zero flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.c) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jpe_code(op_label, syntax, num_bytes):
    """
    JPE instruction is used to jump to a specific instruction in the program if the Parity flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.p) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jpo_code(op_label, syntax, num_bytes):
    """
    JPO instruction is used to jump to a specific instruction in the program if the Parity flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.p) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jp_code(op_label, syntax, num_bytes):
    """
    JP instruction is used to jump to a specific instruction in the program if the Sign flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.s) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_jm_code(op_label, syntax, num_bytes):
    """
    JM instruction is used to jump to a specific instruction in the program if the Sign flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.s) break;\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

"""
CALL instructions
==================
"""

def generate_call_code(op_label, syntax, num_bytes):
    """
    CALL this function is used to switch context when calling a function.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cz_code(op_label, syntax, num_bytes):
    """
    CZ this function is used to switch context when calling a function, if the Zero flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.z) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cnz_code(op_label, syntax, num_bytes):
    """
    CNZ this function is used to switch context when calling a function, if the Zero flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.z) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cc_code(op_label, syntax, num_bytes):
    """
    CC this function is used to switch context when calling a function, if the Zero flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.c) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cnc_code(op_label, syntax, num_bytes):
    """
    CNC this function is used to switch context when calling a function, if the Zero flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.c) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cpe_code(op_label, syntax, num_bytes):
    """
    CPE this function is used to switch context when calling a function, if the Parity flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.p) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cpo_code(op_label, syntax, num_bytes):
    """
    CPO this function is used to switch context when calling a function, if the Parity flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.p) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cp_code(op_label, syntax, num_bytes):
    """
    CP this function is used to switch context when calling a function, if the Sign flag is inactive.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (cpu->flags.s) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_cm_code(op_label, syntax, num_bytes):
    """
    CM this function is used to switch context when calling a function, if the Sign flag is active.
    """
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tif (!cpu->flags.s) break;\n"\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = opcode[2];\n"\
                  + "\tcpu->pc = (cpu->pc << 8) | opcode[1];\n"\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction

def generate_rst_code(op_label, syntax, num_bytes):
    """
    RST instructions are used to jump to specific locations in code.
    """
    rst_num = int(re.match('RST (?P<num>[0-7]).*', syntax).group('num'))
    c_instruction = "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
                  + "{\n"\
                  + "\tcpu->pc += {};\n".format(num_bytes)\
                  + "\tuint8_t pcl = cpu->pc & 0xff;\n"\
                  + "\tuint8_t pch = (cpu->pc >> 8) & 0xff;\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pch);\n"\
                  + "\tcpu->sp--; writeCpu8080Memory(cpu, cpu->sp, pcl);\n"\
                  + "\tcpu->pc = {};\n".format(hex(rst_num*8))\
                  + "\tbreak;\n"\
                  + "}"
    return c_instruction
