#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>

#include "display.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct CHIP8 {
	u8 memory[4096];  // 4 kilobytes of memory, 0x1FF - 0xFFF is where the program/game is stored
    u8 registers[16]; // 16 8-bit registers, register VF (16) is a special type of register and is generally only used by the interpreter
    u16 stack[16];    // Stores subroutines called by the program
	u16 keys[16];

    u8 stack_count;    // Stores how many subroutine addresses are on the stack
    u16 program_counter; // Address of the current instruction being read by the interpreter
    u16 index_register;  // Special 16-bit register

    u8 delay_timer; // 8-bit timer, decremented at 60hz until it reaches zero
    u8 sound_timer; // 8-bit timer, decremented at 60hz until it reaches zero but plays a noise when non-zero

	unsigned int program_length; // Length/Size of the program being run in the CHIP-8
} CHIP8;

// Initialize a CHIP8 struct
CHIP8 chip8_init(u8* program, size_t program_size);

// Runs the next instruction and checks for input
void tick(CHIP8* chip8, Display* display);

// Returns the instruction the program counter is pointing to
u16 fetch_instruction(CHIP8* chip8);

// Decode an instruction and execute it
// Return: true if instruction is a valid CHIP-8 instruction, false otherwise
bool decode_and_execute(CHIP8* chip8, Display* display, u16 instruction);
#endif
