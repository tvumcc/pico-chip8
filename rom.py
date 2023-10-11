# Takes in CHIP-8 ROM files and puts them all into a C file to be used by Pico CHIP-8
from os import listdir, path

class Program:
    def __init__(self, n, i, s):
        self.name = n
        self.identifier = i
        self.size = s

roms_c_out = """#include \"roms.h\"\n"""
roms_h_out = """#include \"program.h\"\n"""
programs = []
counter = 0  # counts how many programs there are

for filename in listdir("./roms"):
    if path.isfile(file_path := path.join("./roms", filename)):
        with open(file_path, "rb") as bin_file:
            bytes = bin_file.read()
            roms_h_out += f"extern unsigned char rom{counter}[{len(bytes)}];\n"
            programs.append(Program('"' + filename.split(".")[0][0:15] + '"', f"rom{counter}", len(bytes)))

            # Write the binary data into a C array
            roms_c_out += f"unsigned char rom{counter}[{len(bytes)}] = " + "{"
            roms_c_out += ','.join(hex(byte) for byte in bytes)
            roms_c_out += "};\n"
    counter += 1

roms_h_out += f"extern Program chip8_roms[{len(programs)}];"
roms_c_out += f"Program chip8_roms[{len(programs)}] = " + "{\n"
roms_c_out += ',\n'.join("\t{" + f"{program.name}, {program.identifier}, {program.size}" + "}" for program in programs)
roms_c_out += "};\n"

# write roms_c_out and roms_f_out
with open("./src/roms.c", "w") as roms_c_file:
    roms_c_file.write(roms_c_out)

with open("./include/roms.h", "w") as roms_h_file:
    roms_h_file.write(roms_h_out)
