#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "display.h"

Display display_init(uint16_t bg, uint16_t fg) {
    Display display;
	display.bg_color = bg;
	display.fg_color = fg;
	fillScreen(display.bg_color);
	display_clear(&display);
    return display;
}

void display_draw(Display* display) {
	for (int y = 0; y < CHIP8_HEIGHT; y++) {
		for (int x = 0; x < CHIP8_WIDTH; x++) {
			if (display->prev_buffer[y][x] != display->pixelArray[y][x]) {
				uint16_t color = display->pixelArray[y][x] == 0xFF ? display->fg_color : display->bg_color;
				fillRect(x*2+1, y*2+32, 2, 2, color);
			}
			display->prev_buffer[y][x] = display->pixelArray[y][x];
		}
	}
}   

bool display_set_pixel(Display* display, int x, int y) {
	bool ret = display->pixelArray[y][x];
	display->pixelArray[y][x] ^= 0xFF;
	return ret;
}

void display_clear(Display* display) {
	for (int y = 0; y < CHIP8_HEIGHT; y++) {
		for (int x = 0; x < CHIP8_WIDTH; x++) {
			display->pixelArray[y][x] = 0x00;
			display->prev_buffer[y][x] = 0x00;
		}
	}
	fillRect(0, 32, 130, 64, display->bg_color);
}
