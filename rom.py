# Takes in CHIP-8 ROM files and puts them all into a C file to be used by Pico CHIP-8
import os


class Program:
    def __init__(self, n, i, s):
        self.name = n
        self.identifier = i
        self.size = s


roms_c = open("./src/roms.c", "w")
roms_h = open("./include/roms.h", "w")

roms_c.write("#include \"roms.h\"\n")
roms_h.write("#include \"program.h\"\n")

programs = []


counter = 0  # counts how many programs there are

for filename in os.listdir("./roms"):
    file_path = os.path.join("./roms", filename)
    if os.path.isfile(file_path):

        with open(file_path, "rb") as bin_file:
            bytes = bin_file.read()
            roms_h.write(f"extern unsigned char rom{counter}[{len(bytes)}];\n")
            programs.append(Program('"' + filename.split(".")[0][0:15] + '"', f"rom{counter}", len(bytes)))

            # Write the binary data into a C array
            roms_c.write(f"unsigned char rom{counter}[{len(bytes)}] = " + "{")
            for byte in bytes:
                roms_c.write(hex(byte) + ",")
            roms_c.write("};\n")

    counter += 1

roms_h.write(f"extern Program chip8_roms[{len(programs)}];")
roms_c.write(f"Program chip8_roms[{len(programs)}] = " + "{\n")
for program in programs:
    roms_c.write("\t{" + f"{program.name}, {program.identifier}, {program.size}" + "},\n")
roms_c.write("};\n")

roms_c.close()
roms_h.close()
