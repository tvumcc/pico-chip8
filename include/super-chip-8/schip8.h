#ifndef SCHIP8_H
#define SCHIP8_H
#include "chip-8/chip8.h"

typedef struct SCHIP8 {
	CHIP8 chip8;
	u8 flags[16];
} SCHIP8;

// Initialize a SCHIP8 struct
SCHIP8 schip8_init(u8* program, size_t program_size);

void tick(SCHIP* schip8, Display* display, Device* device);

void decode_and_execute(SCHIP8* schip8, Display* display, u16 instruction, Device* device);
#endif
