#ifndef CHIP8_OPCODES_H
#define CHIP8_OPCODES_H
#include <stdint.h>

#include "display.h"
#include "chip8.h"


// Documentation comments from https://en.wikipedia.org/wiki/CHIP-8

// Clears the screen
void op_00E0(CHIP8* chip8, Display* display);

// Returns from subroutine
void op_00EE(CHIP8* chip8);

// Jumps to address
void op_1NNN(CHIP8* chip8, u16 NNN);

// Calls subroutine at NNN 
void op_2NNN(CHIP8* chip8, u16 NNN);

// Skips the next instruction if VX equals NN
void op_3XNN(CHIP8* chip8, u8 X, u8 NN);

// Skips the next instruction if VX does not equal NN 
void op_4XNN(CHIP8* chip8, u8 X, u8 NN);

// Skips the next instruction if VX equals VY
void op_5XY0(CHIP8* chip8, u8 X, u8 Y);

// Sets VX to NN
void op_6XNN(CHIP8* chip8, u8 X, u8 NN);

// Adds NN to VX (Carry flag is not changed)
void op_7XNN(CHIP8* chip8, u8 X, u8 NN);

// Sets VX to the value of VY
void op_8XY0(CHIP8* chip8, u8 X, u8 Y);

// Sets VX to VX or VY (bitwise OR)
void op_8XY1(CHIP8* chip8, u8 X, u8 Y);

// Sets VX to VX and VY (bitwise AND)
void op_8XY2(CHIP8* chip8, u8 X, u8 Y);

// Sets VX to VX xor VY (bit wise XOR)
void op_8XY3(CHIP8* chip8, u8 X, u8 Y);

// Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not
void op_8XY4(CHIP8* chip8, u8 X, u8 Y);

// VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not
void op_8XY5(CHIP8* chip8, u8 X, u8 Y);

// Stores the least significant bit of VX in VF and then shifts VX to the right by 1
void op_8XY6(CHIP8* chip8, u8 X, u8 Y);

// Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not
void op_8XY7(CHIP8* chip8, u8 X, u8 Y);

// Stores the most significant bit of VX in VF and then shifts VX to the left by 1
void op_8XYE(CHIP8* chip8, u8 X, u8 Y);

// Skips the next instruction if VX does not equal VY
void op_9XY0(CHIP8* chip8, u8 X, u8 Y);

// Sets I to the address NNN
void op_ANNN(CHIP8* chip8, u16 NNN);

// Jumps to the address NNN plus V0 
void op_BNNN(CHIP8* chip8, u16 NNN);

// Sets VX to the result of a bitwise and operation on a random number (0 - 255) and NN
void op_CXNN(CHIP8* chip8, u8 X, u8 NN);

// Draws a sprite at coordinate (VX, VY) with a width of 8 pixels and height of N pixels
void op_DXYN(CHIP8* chip8, Display* display, u8 X, u8 Y, u8 N);

// Skips the next instruction if the key stored in VX is pressed
void op_EX9E(CHIP8* chip8, u8 X);

// Skips the next instruction if the key stored in VX is not pressed
void op_EXA1(CHIP8* chip8, u8 X);

// Sets VX to the value of the delay timer
void op_FX07(CHIP8* chip8, u8 X);

// A key press is awaited, and then stored in VX (Blocking Operation. All instruction halted until next key event)
void op_FX0A(CHIP8* chip8, u8 X);

// Sets the delay timer to VX
void op_FX15(CHIP8* chip8, u8 X);

// Sets the sound timer to VX
void op_FX18(CHIP8* chip8, u8 X);

// Adds VX to I. VF is not affected
void op_FX1E(CHIP8* chip8, u8 X);

// Sets I to the location of the sprite for the character in VX. Characters 0-F are represented by a 4x5 font
void op_FX29(CHIP8* chip8, u8 X);

/* (FX33) Stores the binary-code decimal representation of VX, with the most significant of the three digits at
the address in I, the middle digit at I plus1, and the least significant digit at I plus 2.
(In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I
the tens digit at location I+1, and the ones digit at location I+2)*/
void op_FX33(CHIP8* chip8, u8 X);

/* (FX55) Stores from V0 to VX (including VX) in memory, starting at address I.
The offset from I is increased by 1 for each value written, but I itself is left unmodified*/
void op_FX55(CHIP8* chip8, u8 X);

/* Fills from V0 to VX (including VX) with values from memory, starting at address I.
The offset from I is increased by 1 for each value read, but I itself it left unmodified*/
void op_FX65(CHIP8* chip8, u8 X);
#endif
