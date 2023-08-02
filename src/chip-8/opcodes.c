#include <stdint.h>

#include "chip-8/display.h"
#include "chip-8/opcodes.h"
#include "chip-8/chip8.h"

#define CHIP8_DEBUG

void op_00E0(CHIP8* chip8, Display* display) {
	display_clear(display);	
}

void op_00EE(CHIP8* chip8) {
	unsigned short return_address = chip8->stack[chip8->stack_count - 1];
	chip8->program_counter = return_address + 2;
	chip8->stack[chip8->stack_count - 1] = 0;
	chip8->stack_count--;
}

void op_1NNN(CHIP8* chip8, uint16_t NNN) {
	chip8->program_counter = NNN;
}

void op_2NNN(CHIP8* chip8, uint16_t NNN) {
	// Push the current program counter onto the stack so that it can be returned to later
	chip8->stack[chip8->stack_count] = chip8->program_counter - 2;
	chip8->stack_count++;
	// Jump to the memory address
	chip8->program_counter = NNN;
}

void op_3XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	if (chip8->registers[X] == NN) 
		chip8->program_counter += 2;
}

void op_4XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	if (chip8->registers[X] != NN)
		chip8->program_counter += 2;
}

void op_5XY0(CHIP8* chip8, uint8_t X, uint8_t Y) {
	if (chip8->registers[X] == chip8->registers[Y])
		chip8->program_counter += 2;
}

void op_6XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	chip8->registers[X] = NN;
}

void op_7XNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	chip8->registers[X] += NN;
}

void op_8XY0(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] = chip8->registers[Y];
}

void op_8XY1(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] |= chip8->registers[Y];
	chip8->registers[15] = 0;
}

void op_8XY2(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] &= chip8->registers[Y];
	chip8->registers[15] = 0;
}

void op_8XY3(CHIP8* chip8, uint8_t X, uint8_t Y) {
	chip8->registers[X] ^= chip8->registers[Y];
	chip8->registers[15] = 0;
}

void op_8XY4(CHIP8* chip8, uint8_t X, uint8_t Y) {
	int result = chip8->registers[X] + chip8->registers[Y];
	if (result > 255) {
		chip8->registers[15] = 1;
		result = (result % 255) - 1;
	} else {
		chip8->registers[15] = 0;
	}

	chip8->registers[X] = (uint8_t)(result);
}

void op_8XY5(CHIP8* chip8, uint8_t X, uint8_t Y) {
	int result = chip8->registers[X] - chip8->registers[Y];
	chip8->registers[15] = 1;

	if (chip8->registers[X] < chip8->registers[Y]) {
		result = 256 + result;
		chip8->registers[15] = 0;
	}

	chip8->registers[X] = (uint8_t)(result);
}

void op_8XY6(CHIP8* chip8, uint8_t X) {
	uint8_t lsb = chip8->registers[X] & 0x01;
	chip8->registers[X] >>= 1;
	chip8->registers[15] = lsb == 1 ? 1 : 0;	
}

void op_8XY7(CHIP8* chip8, uint8_t X, uint8_t Y) {
	int result = chip8->registers[Y] - chip8->registers[X];
	chip8->registers[15] = 1;
	if (chip8->registers[Y] < chip8->registers[X]) {
		result = 256 + result;
		chip8->registers[15] = 0;
	}
	chip8->registers[X] = (uint8_t)(result);
}

void op_8XYE(CHIP8* chip8, uint8_t X) {
	uint8_t msb = (chip8->registers[X] & 0x80) >> 7; 
	chip8->registers[X] <<= 1;
	chip8->registers[15] = msb == 1 ? 1 : 0;
}

void op_9XY0(CHIP8* chip8, uint8_t X, uint8_t Y) {
	if (chip8->registers[X] != chip8->registers[Y])
		chip8->program_counter += 2;
}

void op_ANNN(CHIP8* chip8, uint16_t NNN) {
	chip8->index_register = NNN;
}

void op_BNNN(CHIP8* chip8, uint16_t NNN) {
	chip8->program_counter = (NNN + chip8->registers[0]) - 1;
}

void op_CXNN(CHIP8* chip8, uint8_t X, uint8_t NN) {
	chip8->registers[X] = (uint8_t)((rand() % 256) - 1) & NN;
}

void op_DXYN(CHIP8* chip8, Display* display, uint8_t X, uint8_t Y, uint8_t N) {
	chip8->registers[15] = 0;

	int y_pos = chip8->registers[Y] % 32;
	for (uint8_t i = 0; i < N; i++) {
		int x_pos = chip8->registers[X] % 64;
		uint8_t sprite_byte = chip8->memory[chip8->index_register + i];
		for (int j = 0; j < 8; j++) {
			if (x_pos > 63) {
				break;
			}
				
			if (((sprite_byte & (0x80 >> j)) >> (7 - j)) == 1) // Check if the bit needed is set to 1, if so draw it
				if (display_set_pixel(display, x_pos, y_pos))
					chip8->registers[15] = 1;
			x_pos++;
		}
		y_pos++;
		
	}
	display_draw(display);
}

void op_EX9E(CHIP8* chip8, uint8_t X) {
	if (chip8->keys[chip8->registers[X]] == 1) {
		chip8->program_counter += 2;
	}
}

void op_EXA1(CHIP8* chip8, uint8_t X) {
	if (chip8->keys[chip8->registers[X]] == 0)
		chip8->program_counter += 2;
}

void op_FX07(CHIP8* chip8, uint8_t X) {
	chip8->registers[X] = chip8->delay_timer;	
}

void op_FX0A(CHIP8* chip8, uint8_t X) {
	for (int i = 0; i < 16; i++) {
		if (chip8->keys[i] == 1) {
			chip8->registers[X] = chip8->keys[i];
			return;
		}	
	}
	chip8->program_counter -= 2;
}

void op_FX15(CHIP8* chip8, uint8_t X) {
	chip8->delay_timer = chip8->registers[X];
}

void op_FX18(CHIP8* chip8, uint8_t X) {
	chip8->sound_timer = chip8->registers[X];
}

void op_FX1E(CHIP8* chip8, uint8_t X) {
	chip8->index_register += chip8->registers[X];	
}

void op_FX29(CHIP8* chip8, uint8_t X) {
	chip8->index_register = chip8->registers[X] * 5;
}

void op_FX33(CHIP8* chip8, uint8_t X) {
	uint8_t number = chip8->registers[X];
	uint8_t digit1 = number / 100;
	uint8_t digit2 = (number % 100) / 10;
	uint8_t digit3 = number % 10;
	chip8->memory[chip8->index_register] = digit1;
	chip8->memory[chip8->index_register + 1] = digit2;
	chip8->memory[chip8->index_register + 2] = digit3;
}

void op_FX55(CHIP8* chip8, uint8_t X) {
	for (uint8_t i = 0; i <= X; i++) {
		chip8->memory[chip8->index_register + i] = chip8->registers[i];
	}
}

void op_FX65(CHIP8* chip8, uint8_t X) {
	for (uint8_t i = 0; i <= X; i++) {
		chip8->registers[i] = chip8->memory[chip8->index_register + i];
	}
}
