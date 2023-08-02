#ifndef CHIP8_H
#define CHIP8_H
#include "display.h"

typedef struct CHIP8 {
	unsigned char memory[4096];  // 4 kilobytes of memory, 0x1FF - 0xFFF is where the program/game is stored
    unsigned char registers[16]; // 16 8-bit registers, register VF (16) is a special type of register and is generally only used by the interpreter
    unsigned short stack[16];    // Stores subroutines called by the program
	unsigned char keys[16];

    unsigned char stack_count;    // Stores how many subroutine addresses are on the stack
    unsigned short program_counter; // Address of the current instruction being read by the interpreter
    unsigned short index_register;  // Special 16-bit register

    unsigned char delay_timer; // 8-bit timer, decremented at 60hz until it reaches zero
    unsigned char sound_timer; // 8-bit timer, decremented at 60hz until it reaches zero but plays a noise when non-zero

	unsigned int program_length; // Length/Size of the program being run in the CHIP-8
} CHIP8;

// Initialize a CHIP8 struct
CHIP8 chip8_init(unsigned char* program, size_t program_size);

// Loads a program from a file path into memory
void read_program_to_memory(CHIP8* chip8, const char* program_path);

void tick(CHIP8* chip8, Display* display);

// Get the instruction the program counter is pointing to
unsigned short fetch_instruction(CHIP8* chip8);

// Decode an instruction and execute it
void decode_and_execute(CHIP8* chip8, Display* display, unsigned short instruction);
#endif
