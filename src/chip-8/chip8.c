#include <math.h>

#include "chip-8/chip8.h"
#include "chip-8/chip8_opcodes.h"
#include "chip-8/schip8_opcodes.h"
#include "chip-8/xochip_opcodes.h"

void chip8_init(CHIP8* chip8, u8* program, size_t program_size) {
	u8 fontset[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80, // F
	};


	// Initialize memory to 0
	for (int i = 0; i < 4096; i++) {
		// Load the first 80 bytes with the fontset
		if (i < 80) {
			chip8->memory[i] = fontset[i];
		} else if (i < 512) { // Reserved memory has to have some sort of data (needed for roms such as Binding of Cosmac)
			chip8->memory[i] = (u8)(rand() % 256);
		}

	}

	// Big hex font
	for (int k = 0, i = 80; k < 80; i+=2, k++) {
		u8 out = 0;
		for (int j = 0; j < 4; j++) {
			out |= ((chip8->memory[k] & (1 << (j+4))) ? (3 << (j * 2)) : (0));
		}
		chip8->memory[i] = out;
		chip8->memory[i+1] = out;
	}

	// Load program into memory
	for (int i = 0; i < program_size; i++) {
		chip8->memory[0x200 + i] = program[i];
	}

	// Initialize registers and stack to 0
	for (int i = 0; i < 16; i++) {
		chip8->registers[i] = 0;
		chip8->stack[i] = 0;
		chip8->keys[i] = 0;
		chip8->flags[i] = 0;
		chip8->audio_pattern_buffer[i] = 0;
	}


	// Initialize the rest of the values
	chip8->stack_count = 0;
	chip8->program_counter = 0x200;
	chip8->index_register = 0;
	chip8->delay_timer = 0;
	chip8->sound_timer = 0;
	chip8->audio_playback_rate = 4000;
}

void tick(CHIP8* chip8, Display* display) {
	u16 instruction = fetch_instruction(chip8);
	decode_and_execute(chip8, display, instruction);
}

void timer_tick(CHIP8* chip8) {
	if (chip8->sound_timer > 0)	
		chip8->sound_timer--;
	if (chip8->delay_timer > 0)
		chip8->delay_timer--;
}

u16 fetch_instruction(CHIP8* chip8) {
	if (chip8->program_counter <= 0xFFFF) {
		unsigned short instruction = (chip8->memory[chip8->program_counter] << 8) | chip8->memory[chip8->program_counter + 1];
		chip8->program_counter += 2;
		return instruction;
	} else {
		return 0x200;
	}
}

void decode_and_execute(CHIP8* chip8, Display* display, u16 instruction) {
	// Possible Arguments
	u8 X   = (instruction & 0x0F00) >> 8;
	u8 Y   = (instruction & 0x00F0) >> 4;
	u8 N   = (instruction & 0x000F);
	u8 NN  = (instruction & 0x00FF);
	u16 NNN = (instruction & 0x0FFF);
		
	switch((instruction & 0xF000) >> 12) {
		case 0x00:
			switch(NN) {
				case 0xE0:
					op_00E0(chip8, display);
					break;
				case 0xEE:
					op_00EE(chip8);
					break;
				case 0xFF:
					op_00FF(display);
					break;
				case 0xFE:
					op_00FE(display);
					break;
				case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
				case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
					op_00CN(display, N);
					break;
				case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
				case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
					op_00DN(display, N);
					break;
				case 0xFB:
					op_00FB(display);
					break;
				case 0xFC:
					op_00FC(display);
					break;
				case 0xFD:
					op_00FD();
					break;
			}
			break;
		case 0x01:
			op_1NNN(chip8, NNN);
			break;
		case 0x02:
			op_2NNN(chip8, NNN);
			break;
		case 0x03:
			op_3XNN(chip8, X, NN);
			break;
		case 0x04:
			op_4XNN(chip8, X, NN);
			break;
		case 0x05:
			switch (N) {
				case 0x00:
					op_5XY0(chip8, X, Y);
					break;
				case 0x02:
					op_5XY2(chip8, X, Y);
					break;
				case 0x03:
					op_5XY3(chip8, X, Y);
					break;
			}
			break;
		case 0x06: 
			op_6XNN(chip8, X, NN);
			break;
		case 0x07: 
			op_7XNN(chip8, X, NN);
			break;
		case 0x08:
		   	switch(N) {
				case 0: 
					op_8XY0(chip8, X, Y);
					break;
				case 1:
					op_8XY1(chip8, X, Y);
					break;
				case 2:
					op_8XY2(chip8, X, Y);
					break;
				case 3:
					op_8XY3(chip8, X, Y);
					break;
				case 4:
					op_8XY4(chip8, X, Y);
					break;
				case 5:
					op_8XY5(chip8, X, Y);
					break;
				case 6:
					op_8XY6(chip8, X, Y);
					break;
				case 7:
					op_8XY7(chip8, X, Y);
					break;
				case 14:
					op_8XYE(chip8, X, Y);
					break;
		   	}
			break;
		case 0x09:
			op_9XY0(chip8, X, Y);
			break;
		case 0x0a:
			op_ANNN(chip8, NNN);
			break;
		case 0x0b:
			op_BNNN(chip8, NNN);	
			break;
		case 0x0c:
			op_CXNN(chip8, X, NN);
			break;	
		case 0x0d:
			if (N == 0) {
				op_DXY0(chip8, display, X, Y);
			} else {
				op_DXYN(chip8, display, X, Y, N);
			}
			break;
		case 0x0e:
			switch(NN) {
				case 0x9e:
					op_EX9E(chip8, X);
					break;
				case 0xa1:
					op_EXA1(chip8, X);
					break;
			}
			break;
		case 0x0f:
			switch(NN) {
				case 0x00:
					if (NNN == 0x000)
						op_F000NNNN(chip8);
					break;
				case 0x01:
					op_FN01(display, X);
					break;
				case 0x02:
					if (NNN == 0x002) 
						op_F002(chip8);
					break;
				case 0x07:
					op_FX07(chip8, X);
					break;
				case 0x0a:
					op_FX0A(chip8, X);
					break;
				case 0x15:
					op_FX15(chip8, X);
					break;
				case 0x18:
					op_FX18(chip8, X);
					break;
				case 0x1e:
					op_FX1E(chip8, X);
					break;
				case 0x29:
					op_FX29(chip8, X);
					break;
				case 0x33:
					op_FX33(chip8, X);
					break;
				case 0x3A:
					op_FX3A(chip8, X);
					break;
				case 0x55:
					op_FX55(chip8, X);
					break;
				case 0x65:
					op_FX65(chip8, X);
					break;
				case 0x75:
					op_FX75(chip8, X);
					break;
				case 0x85:
					op_FX85(chip8, X);
					break;
			}
			break;
	}
}
