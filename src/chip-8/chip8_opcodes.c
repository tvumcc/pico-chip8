#include "display.h"

#include "chip-8/chip8.h"
#include "chip-8/chip8_opcodes.h"

void skip_next_instruction(CHIP8* chip8) {
	if (chip8->memory[chip8->program_counter] == 0xF0 && chip8->memory[chip8->program_counter + 1] == 0x00) {
		chip8->program_counter += 4;
	} else {
		chip8->program_counter += 2;
	}
}

void op_00E0(CHIP8* chip8, Display* display) {
	display_clear(display);	
}

void op_00EE(CHIP8* chip8) {
	unsigned short return_address = chip8->stack[chip8->stack_count - 1];
	chip8->program_counter = return_address + 2;
	chip8->stack[chip8->stack_count - 1] = 0;
	chip8->stack_count--;
}

void op_1NNN(CHIP8* chip8, u16 NNN) {
	chip8->program_counter = NNN;
}

void op_2NNN(CHIP8* chip8, u16 NNN) {
	// Push the current program counter onto the stack so that it can be returned to later
	chip8->stack[chip8->stack_count] = chip8->program_counter - 2;
	chip8->stack_count++;
	// Jump to the memory address
	chip8->program_counter = NNN;
}

void op_3XNN(CHIP8* chip8, u8 X, u8 NN) {
	if (chip8->registers[X] == NN) skip_next_instruction(chip8);
}

void op_4XNN(CHIP8* chip8, u8 X, u8 NN) {
	if (chip8->registers[X] != NN) skip_next_instruction(chip8);
}

void op_5XY0(CHIP8* chip8, u8 X, u8 Y) {
	if (chip8->registers[X] == chip8->registers[Y]) skip_next_instruction(chip8);
}

void op_6XNN(CHIP8* chip8, u8 X, u8 NN) {
	chip8->registers[X] = NN;
}

void op_7XNN(CHIP8* chip8, u8 X, u8 NN) {
	chip8->registers[X] += NN;
}

void op_8XY0(CHIP8* chip8, u8 X, u8 Y) {
	chip8->registers[X] = chip8->registers[Y];
}

void op_8XY1(CHIP8* chip8, u8 X, u8 Y) {
	chip8->registers[X] |= chip8->registers[Y];
	chip8->registers[15] = 0;
}

void op_8XY2(CHIP8* chip8, u8 X, u8 Y) {
	chip8->registers[X] &= chip8->registers[Y];
	chip8->registers[15] = 0;
}

void op_8XY3(CHIP8* chip8, u8 X, u8 Y) {
	chip8->registers[X] ^= chip8->registers[Y];
	chip8->registers[15] = 0;
}

void op_8XY4(CHIP8* chip8, u8 X, u8 Y) {
	int result = (int)chip8->registers[X] + (int)chip8->registers[Y];

	if (result > 255) {
		result %= 256;
		chip8->registers[X] = (u8)(result);
		chip8->registers[15] = 1;
	} else {
		chip8->registers[X] = (u8)(result);
		chip8->registers[15] = 0;
	}

}

void op_8XY5(CHIP8* chip8, u8 X, u8 Y) {
	int result = (int)chip8->registers[X] - (int)chip8->registers[Y];

	if (chip8->registers[X] < chip8->registers[Y]) {
		result %= 256;
		chip8->registers[X] = (u8)(result);
		chip8->registers[15] = 0;
	} else {
		chip8->registers[X] = (u8)(result);
		chip8->registers[15] = 1;
	}

}

void op_8XY6(CHIP8* chip8, u8 X, u8 Y) {
	chip8->registers[X] = chip8->registers[Y];
	u8 lsb = chip8->registers[X] & 0x01;
	chip8->registers[X] >>= 1;
	chip8->registers[15] = lsb == 1 ? 1 : 0;	
}

void op_8XY7(CHIP8* chip8, u8 X, u8 Y) {
	int result = (int)chip8->registers[Y] - (int)chip8->registers[X];

	if (chip8->registers[Y] < chip8->registers[X]) {
		result %= 256;
		chip8->registers[X] = (u8)(result);
		chip8->registers[15] = 0;
	} else {
		chip8->registers[X] = (u8)(result);
		chip8->registers[15] = 1;
	}
}

void op_8XYE(CHIP8* chip8, u8 X, u8 Y) {
	chip8->registers[X] = chip8->registers[Y];
	u8 msb = (chip8->registers[X] & 0x80) >> 7; 
	chip8->registers[X] <<= 1;
	chip8->registers[15] = msb == 1 ? 1 : 0;
}

void op_9XY0(CHIP8* chip8, u8 X, u8 Y) {
	if (chip8->registers[X] != chip8->registers[Y]) skip_next_instruction(chip8);
}

void op_ANNN(CHIP8* chip8, u16 NNN) {
	chip8->index_register = NNN;
}

void op_BNNN(CHIP8* chip8, u16 NNN) {
	chip8->program_counter = (NNN + chip8->registers[0]);
}

void op_CXNN(CHIP8* chip8, u8 X, u8 NN) {
	chip8->registers[X] = (u8)((rand() % 256)) & NN;
}

void op_DXYN(CHIP8* chip8, Display* display, u8 X, u8 Y, u8 N) {
	chip8->registers[15] = 0;
	int y_pos = chip8->registers[Y] % CHIP8_HEIGHT;

	for (u8 i = 0; i < N; i++) {
		if (y_pos >= CHIP8_HEIGHT) break;
		int x_pos = chip8->registers[X] % CHIP8_WIDTH;
		u8 row = chip8->memory[chip8->index_register + i];

		for (int j = 7; j >= 0; j--) {
			if (x_pos >= CHIP8_WIDTH) break;
			if (row & (1 << j)) // Check if the bit needed is set to 1, if so draw it
				if (display_set_pixel(display, x_pos, y_pos))
					chip8->registers[15] = 1;
			x_pos++;
		}

		y_pos++;
	}

	display_draw(display);
}

void op_EX9E(CHIP8* chip8, u8 X) {
	if (chip8->keys[chip8->registers[X]] == 1) {
		skip_next_instruction(chip8);
	}
}

void op_EXA1(CHIP8* chip8, u8 X) {
	if (chip8->keys[chip8->registers[X]] == 0) {
		skip_next_instruction(chip8);
	}
}

void op_FX07(CHIP8* chip8, u8 X) {
	chip8->registers[X] = chip8->delay_timer;	
}

void op_FX0A(CHIP8* chip8, u8 X) {
	for (int i = 0; i < 16; i++) {
		if (chip8->keys[i] == 1) {
			chip8->registers[X] = chip8->keys[i];
			return;
		}	
	}
	chip8->program_counter -= 2;
}

void op_FX15(CHIP8* chip8, u8 X) {
	chip8->delay_timer = chip8->registers[X];
}

void op_FX18(CHIP8* chip8, u8 X) {
	chip8->sound_timer = chip8->registers[X];
}

void op_FX1E(CHIP8* chip8, u8 X) {
	chip8->index_register += chip8->registers[X];	
}

void op_FX29(CHIP8* chip8, u8 X) {
	chip8->index_register = chip8->registers[X] * 5;
}

void op_FX33(CHIP8* chip8, u8 X) {
	u8 number = chip8->registers[X];
	u8 digit1 = number / 100;
	u8 digit2 = (number % 100) / 10;
	u8 digit3 = number % 10;
	chip8->memory[chip8->index_register] = digit1;
	chip8->memory[chip8->index_register + 1] = digit2;
	chip8->memory[chip8->index_register + 2] = digit3;
}

void op_FX55(CHIP8* chip8, u8 X) {
	for (u8 i = 0; i <= X; i++) {
		chip8->memory[chip8->index_register++] = chip8->registers[i];
	}
}

void op_FX65(CHIP8* chip8, u8 X) {
	for (u8 i = 0; i <= X; i++) {
		chip8->registers[i] = chip8->memory[chip8->index_register++];
	}
}
