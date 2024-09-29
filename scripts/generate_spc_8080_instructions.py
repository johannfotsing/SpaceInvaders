def generate_nop_code(op_label, syntax, num_bytes):
    r"""
    NOP instruction is a void instruction that can be used for waiting.
    """
    return "case {}: cpu->program_counter += {}; break;\t\t\t\t\t\t// {}".format(op_label, num_bytes, syntax)

def generate_ei_code(op_label, syntax, num_bytes):
    """
    EI instruction is used to enable interruptions.
    """
    return "case {}: cpu->interrupt_enabled = 1; cpu->program_counter += {}; break;\t\t\t\t// {}\n".format(op_label, num_bytes, syntax)

def generate_di_code(op_label, syntax, num_bytes):
    """
    DI instruction is used to enable interruptions.
    """
    return "case {}: cpu->interrupt_enabled = 0; cpu->program_counter += {}; break;\t\t\t\t// {}\n".format(op_label, num_bytes, syntax)

def generate_daa_code(op_label, syntax, num_bytes):
    """
    DAA instruction is a special instruction (Decimal Adjust Accumulator)
    """
    c_instruction = \
        "case {}:\t\t\t\t\t\t// {}\n".format(op_label, syntax)\
      + "{\n"\
      + "\tcpu8080DAA(cpu);\n"\
      + "\tcpu->program_counter += {};\n".format(num_bytes)\
      + "\tbreak;\n"\
      + "}"
    return c_instruction

def generate_hlt_code(op_label, syntax, num_bytes):
    """
    HLT (halt) is used to stop the processor and wait for interruption.
    """
    return "case {}: cpu->program_counter += {}; cpu->stopped = 1; cpu->interrupt_enabled=1; break;\t\t// {}\n".format(op_label, num_bytes, syntax)
