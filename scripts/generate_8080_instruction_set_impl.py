import re
from scripts._8080 import GENERATORS_MAP
    

CPU8080_INSTRUCTION_PARSER = re.compile('(?P<instruction_number>0x.*)[\t]+(?P<instruction_syntax>.*)[\t]+(?P<operation_bytes>[1-3])[\t].*$', re.A)
CPU8080_STRCODE_PARSER = re.compile('(?P<instruction_strcode>[A-Z]+) .*$', re.A)


if __name__ == "__main__":
    
    emu_impl = ""
    with open("./doc/instruction_set_8080.txt") as ops_file:
        
        op_descs = ops_file.readlines()
        
        for i, op_desc in enumerate(op_descs):
            
            res = CPU8080_INSTRUCTION_PARSER.match(op_desc)
            
            instruction_syntax = res.group('instruction_syntax')
            instruction_number = res.group('instruction_number')
            instruction_size = res.group('operation_bytes')
            
            res = CPU8080_STRCODE_PARSER.match(instruction_syntax)
            
            if res is not None:
                instruction_str_code = res.group('instruction_strcode')
            else:
                instruction_str_code = instruction_syntax
            
            if instruction_str_code in GENERATORS_MAP.keys():
                instruction_c_code = GENERATORS_MAP[instruction_str_code](instruction_number, instruction_syntax, instruction_size)
                # print(instruction_c_code)
                emu_impl += ('\n' + instruction_c_code)
            else:
                print(instruction_str_code)
    
    with open("./doc/instruction_set_8080_impl.txt", "w") as gen_file:
        gen_file.write(emu_impl)