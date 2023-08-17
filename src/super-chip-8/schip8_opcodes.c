#include "super-chip-8/schip8_opcodes.h"
#include "device.h"

void op_00FF(SCHIP8* schip8, Display* display) {
	display_hi_res(display);
}

void op_00FE(SCHIP8* schip8, Display* display) {
	display_low_res(display);
}

void op_00CN(SCHIP8* schip8, Display* display, u8 N) {
	int screen_height = display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT;
	int screen_width = display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH;

	for (int x = 0; x < screen_width; x++) {
		for (int y = screen_height-1; y >= 0; y--) {
			if (y < N) display->pixelArray[y][x] = 0x00;
			else {
				display->pixelArray[y][x] = display->pixelArray[y+N][x];
			}
		}
	}
}

void op_00FB(SCHIP8* schip8, Display* display) {
	int screen_height = display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT;
	int screen_width = display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH;

	for (int y = 0; y < screen_height; y++) {
		for (int x = screen_width-1; x >= 0; x--) {
			if (x < 4) display->pixelArray[y][x] = 0x00;
			else {
				display->pixelArray[y][x] = display->pixelArray[y][x-4];
			}
		}
	}
}

void op_00FC(SCHIP8* schip8, Display* display) {
	int screen_height = display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT;
	int screen_width = display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH;
	
	for (int y = 0; y < screen_height; y++) {
		for (int x = 0; x < screen_width; x--) {
			if (x > screen_width-4) display->pixelArray[y][x] = 0x00;
			else {
				display->pixelArray[y][x] = display->pixelArray[y][x+4];
			}
		}
	}
}

void op_00FD(SCHIP8* schip8, Device* device) {
	home_goto(device);	
}

void op_DXY0(SCHIP8* schip8, Display* display, u8 X, u8 Y) {
	chip8->registers[15] = 0;

	int screen_height = display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT;
	int screen_width = display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH;

	int y_pos = schip8->chip8.registers[Y] % screen_height;
	
	for (int i = 0; i < 32; i += 2) {
		if (y_pos >= screen_height) break;
		int x_pos = schip8->chip8.registers[X] % screen_width;
		u16 row = (schip8->chip8.memory[schip8->chip8.index_register + i] << 8) | schip8->chip8.memory[schip8->chip8.index_register + i+1];	

		for (int j = 15; j >= 0; j--) {
			if (x_pos >= screen_width) break;
			if ((row << j) & 1)
				if (display_set_pixel(display, x_pos, y_pos))
					chip8->registers[15] = 1;
			x_pos++;
		}

		y_pos++;
	}		
	display_draw(display);
}

void op_FX30(SCHIP8* schip8, u8 X) {
	schip8->chip8.index_register = 0x50 + (10 * X);
}

void op_FX75(SCHIP8* schip8, u8 X) {
	for (int i = 0; i <= X; i++) {
		schip8.flags[i] = schip8->chip8.registers[i];
	}
}

void op_FX85(SCHIP8* schip8, u8 X) {
	for (int i = 0; i <= X; i++) {
		schip8->chip8.registers[i] = schip8.flags[i];
	}
}
