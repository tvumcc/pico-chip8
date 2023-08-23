#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "display.h"

Display display_init(unsigned short bg, unsigned short bp0, unsigned short bp1, unsigned short blend) {
    Display display;
	display.bitplane_mask = 0x0F;
	display.bg_color = bg;
	display.bitplane0_color = bp0;
	display.bitplane1_color = bp1;
	display.blend_color = blend;
	fillScreen(display.bg_color);

	for (int y = 0; y < SUPER_CHIP8_HEIGHT; y++) {
		for (int x = 0; x < SUPER_CHIP8_WIDTH; x++) {
			display.pixelArray[y][x] = 0x00;
			display.prev_buffer[y][x] = 0x00;
		}
	}

	display.extended_resolution = false;
    return display;
}

void display_draw(Display* display) {
	int screen_height = display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT;
	int screen_width = display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH;
	int pixel_size = display->extended_resolution ? 1 : 2;

	for (int y = 0; y < screen_height; y++) {
		for (int x = 0; x < screen_width; x++) {
			if (display->prev_buffer[y][x] != display->pixelArray[y][x]) {
				switch (display->pixelArray[y][x]) {
					case 0x00:
						fillRect(x*pixel_size+1, y*pixel_size+32, pixel_size, pixel_size, display->bg_color);
						break;
					case 0x0F:
						fillRect(x*pixel_size+1, y*pixel_size+32, pixel_size, pixel_size, display->bitplane0_color);
						break;
					case 0xF0:
						fillRect(x*pixel_size+1, y*pixel_size+32, pixel_size, pixel_size, display->bitplane1_color);
						break;
					case 0xFF:
						fillRect(x*pixel_size+1, y*pixel_size+32, pixel_size, pixel_size, display->blend_color);
						break;
				}
			}
			display->prev_buffer[y][x] = display->pixelArray[y][x];
		}
	}
}   

bool display_set_pixel(Display* display, int x, int y) {
	bool ret = display->pixelArray[y][x] & display->bitplane_mask;
	display->pixelArray[y][x] = display->pixelArray[y][x] ^ display->bitplane_mask;
	return ret;
}

void display_clear(Display* display) {
	for (int y = 0; y < (display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT); y++) {
		for (int x = 0; x < (display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH); x++) {
			display->prev_buffer[y][x] = display->pixelArray[y][x];
			display->pixelArray[y][x] &= ~display->bitplane_mask;
		}
	}
	display_draw(display);
}

void display_hi_res(Display* display) {
	display->extended_resolution = true;
	display_draw(display);
}

void display_low_res(Display* display) {
	display->extended_resolution = false;
	display_draw(display);
}
