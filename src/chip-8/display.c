#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "chip-8/display.h"

Display display_init() {
    Display display;
	display_clear(&display);
    return display;
}

void display_draw(Display* display) {
	for (int y = 0; y < CHIP8_HEIGHT; y++) {
		for (int x = 0; x < CHIP8_WIDTH; x++) {
			if (display->pixelArray[y][x] == 0xFF) {
				drawPixel(x, y, ST7735_WHITE);
			} else {
				drawPixel(x, y, ST7735_BLACK);
			}
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
	for (int y = 0; y < CHIP8_HEIGHT; y++) {
		for (int x = 0; x < CHIP8_WIDTH; x++) {
			display->pixelArray[y][x] = 0x00;
		}
	}
	fillScreen(ST7735_BLACK);
}
