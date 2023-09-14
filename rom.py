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

programs = []


counter = 0  # counts how many programs there are

for filename in os.listdir("./roms"):
    file_path = os.path.join("./roms", filename)
    if os.path.isfile(file_path):

        with open(file_path, "rb") as bin_file:
            bytes = bin_file.read()
            roms_h.write(f"extern unsigned char rom{counter}[{len(bytes)}];")
            programs.append(Program(file_path.split(".")[0], f"rom{counter}", len(bytes)))

            # Write the binary data into a C array
            roms_c.write(f"unsigned char rom{counter}[{len(bytes)}] = " + "{\n")
            for byte in bytes:
                roms_c.write(hex(byte) + ", ")
            roms_c.write("\n};\n")

    counter += 1

roms_c.close()
roms_h.close()
