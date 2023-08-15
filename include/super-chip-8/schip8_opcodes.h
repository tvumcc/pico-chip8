#ifndef SCHIP8_OPCODES_H
#define SCHIP8_OPCODES_H

#include "display.h"

#include "super-chip-8/schip8.h"


// Documentation comments from http://johnearnest.github.io/Octo/docs/SuperChip.html

// Enable 128x64 high resolution graphics mode.(hires)
void op_00FF(SCHIP8* schip8, Display* display);

// Disable high resolution graphics mode and return to 64x32
void op_00FE(SCHIP8* schip8, Display* display);

// Scroll the display down by 0 to 15 pixels
void op_00CN(SCHIP8* schip8, Display* display, u8 N);

// Scroll the display right by 4 pixels
void op_00FB(SCHIP8* schip8, Display* display);

// Scroll the display left by 4 pixels
void op_00FC(SCHIP8* schip8, Display* display);

// Exit the interpreter
void op_00FD(SCHIP8* schip8, Device* device);

// Draw a 16x16 sprite
void op_DXY0(SCHIP8* schip8, Display* display, u8 X, u8 Y);

// Set I to a large hexadecimal character based on the value of VX
void op_FX30(SCHIP8* schip8, u8 X);

// Save V0-VX to flag registers
void op_FX75(SCHIP8* schip8, u8 X);

// Restore V0-VX from flag registers
void op_FX85(SCHIP8* schip8, u8 X);
#endif
