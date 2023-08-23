#ifndef XOCHIP_OPCODES_H
#define XOCHIP_OPCODES_H
#include "display.h"

#include "chip-8/chip8.h"
// Documentation comments from http://johnearnest.github.io/Octo/docs/XO-ChipSpecification.html

// Scroll the contents of the display up by 0-15 pixels
void op_00DN(Display* display, u8 N);

// Save an inclusive range of registers to memory starting at i, does not increment i
void op_5XY2(CHIP8* chip8, u8 X, u8 Y);

// Load an inclusive range of registers from memory starting at i, does not increment i
void op_5XY3(CHIP8* chip8, u8 X, u8 Y);

// Load i with a 16-bit address
void op_F000NNNN(CHIP8* chip8);

// Select zero or more drawing planes by bitmask (0 <= n <= 3)
void op_FN01(Display* display, u8 X);

// Store 16 bytes starting at i in the audio pattern buffer
void op_F002(CHIP8* chip8);

// Set the audio pattern playback rate to `4000*2^((vx-64)/48)` Hz
void op_FX3A(CHIP8* chip8, u8 X);
#endif
