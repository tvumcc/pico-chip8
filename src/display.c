#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "display.h"

Display display_init() {
    Display display;
	fillScreen(ST7735_BLACK);
	display_clear(&display);
    return display;
}

void display_draw(Display* display) {
	for (int y = 0; y < CHIP8_HEIGHT; y++) {
		for (int x = 0; x < CHIP8_WIDTH; x++) {
			if (display->prev_buffer[y][x] != display->pixelArray[y][x]) {
				if (display->pixelArray[y][x] == 0xFF) {
					fillRect(x*2+1, y*2+32, 2, 2, ST7735_WHITE);
				} else {
					fillRect(x*2+1, y*2+32, 2, 2, ST7735_BLACK);
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
	for (int y = 0; y < CHIP8_HEIGHT; y++) {
		for (int x = 0; x < CHIP8_WIDTH; x++) {
			display->pixelArray[y][x] = 0x00;
			display->prev_buffer[y][x] = 0x00;
		}
	}
	fillRect(0, 32, 130, 64, ST7735_BLACK);
}
