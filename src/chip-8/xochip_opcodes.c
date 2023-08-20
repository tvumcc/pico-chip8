#include "chip-8/xochip_opcodes.h"

void op_00DN(Display* display, u8 N) {
	int screen_height = display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT;
	int screen_width = display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH;

	for (int x = 0; x < screen_width; x++) {
		for (int y = screen_height-1; y >= 0; y--) {
			if (y >= screen_height-N) display->pixelArray[y][x] = 0x00;
			else {
				display->pixelArray[y][x] = display->pixelArray[y+N][x];
			}
		}
	}

	display_draw(display);
}

void op_5XY2(CHIP8* chip8, u8 X, u8 Y) {
	for (u8 k = X, i = 0; k <= Y; k++, i++) {
		chip8->memory[chip8->index_register + i] = chip8->registers[k];
	}	
}

void op_5XY3(CHIP8* chip8, u8 X, u8 Y) {
	for (u8 k = X, i = 0; k <= Y; k++, i++) {
		chip8->registers[k] = chip8->memory[chip8->index_register + i];
	}
}

void op_F000NNNN(CHIP8* chip8) {
	chip8->index_register = fetch_instruction(chip8); 
}

void op_FN01(Display* display, u8 X) {
	for (int i = 0; i < 8; i++) display->bitplane_mask |= ((X & (i/4 + 1)) << i)
}

void op_F002(CHIP8* chip8) {
	for (int i = 0; i < 16; i++) {
		chip8->audio_pattern_buffer[i] = chip8->memory[chip8->index_register + i];
	}
}

void op_FX3A(CHIP8* chip8, u8 X) {
	chip8->audio_playback_rate = (u16)floor(4000.0d * (pow(2.0d, ((double)chip8 - 64.0d) / 48.0d)))
}
