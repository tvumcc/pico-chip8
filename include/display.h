#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

typedef struct Display {
    uint8_t pixelArray[CHIP8_HEIGHT][CHIP8_WIDTH];
	uint8_t prev_buffer[CHIP8_HEIGHT][CHIP8_WIDTH];
	
	uint16_t bg_color;
	uint16_t fg_color;
} Display;

// Initialize the display array and colors
Display display_init(uint16_t bg, uint16_t fg);

// Draw the buffer to the screen
void display_draw(Display* display);

// Set pixel of display array
bool display_set_pixel(Display* display, int x, int y);

// Set all pixels of display array to 0x00 (black)
void display_clear(Display* display);
#endif
