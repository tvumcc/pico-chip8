#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>

#include "display.h"

typedef struct CHIP8 {
	uint8_t memory[4096];  // 4 kilobytes of memory
    uint8_t registers[16]; // 16 8-bit registers, register VF (16) is a special type of register and is generally only used by the interpreter
    uint16_t stack[16];    // Stores subroutines called by the program
	uint16_t keys[16];

    uint8_t stack_count;    // Stores how many subroutine addresses are on the stack
    uint16_t program_counter; // Address of the current instruction being read by the interpreter
    uint16_t index_register;  // Special 16-bit register

    uint8_t delay_timer; // 8-bit timer, decremented at 60hz until it reaches zero
    uint8_t sound_timer; // 8-bit timer, decremented at 60hz until it reaches zero. Plays a noise when non-zero
} CHIP8;

// Initialize a CHIP8 struct
void chip8_init(CHIP8* chip8, uint8_t* program, size_t program_size);

// Runs the next instruction. Gets called the clock speed
void tick(CHIP8* chip8, Display* display);

// Decrements the timers if needed, should be called at 60hz
void timer_tick(CHIP8* chip8);

// Returns the instruction the program counter is pointing to
uint16_t fetch_instruction(CHIP8* chip8);

// Decode an instruction and execute it
void decode_and_execute(CHIP8* chip8, Display* display, uint16_t instruction);
#endif
