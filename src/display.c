#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "display.h"

Display display_init() {
    Display display;
	fillScreen(ST7735_BLACK);
	display.extended_resolution = true;
	display_clear(&display);
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
				if (display->pixelArray[y][x] == 0xFF) {
					fillRect(x*pixel_size+1, y*pixel_size+32, pixel_size, pixel_size, ST7735_WHITE);
				} else {
					fillRect(x*pixel_size+1, y*pixel_size+32, pixel_size, pixel_size, ST7735_BLACK);
				}
			}
			display->prev_buffer[y][x] = display->pixelArray[y][x];
		}
	}
}   

bool display_set_pixel(Display* display, int x, int y) {
	unsigned char* pixel = &(display->pixelArray[y][x]);
	if (*pixel == 0xFF) {
		*pixel = 0x00;
		return true; // Flipped
	} else {
		*pixel = 0xFF;
		return false; // Did not flip
	}
}

void display_clear(Display* display) {
	for (int y = 0; y < (display->extended_resolution ? SUPER_CHIP8_HEIGHT : CHIP8_HEIGHT); y++) {
		for (int x = 0; x < (display->extended_resolution ? SUPER_CHIP8_WIDTH : CHIP8_WIDTH); x++) {
			display->pixelArray[y][x] = 0x00;
			display->prev_buffer[y][x] = 0x00;
		}
	}
	fillRect(0, 32, 130, 64, ST7735_BLACK);
}

void display_hi_res(Display* display) {
	display->extended_resolution = true;
	display_draw(display);
}

void display_low_res(Display* display) {
	display->extended_resolution = false;
	display_draw(display);
}
