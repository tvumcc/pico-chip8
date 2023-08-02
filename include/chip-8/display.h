#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32

typedef struct Display {
    unsigned char pixelArray[CHIP8_HEIGHT][CHIP8_WIDTH];
	unsigned char prev_buffer[CHIP8_HEIGHT][CHIP8_WIDTH];
} Display;

// Initialize the display array and texture
Display display_init();

// Draw the display texture to the window
void display_draw(Display* display);

// Set pixel of display array
bool display_set_pixel(Display* display, int x, int y);

// Set all pixels of display array to 0x00 (black)
void display_clear(Display* display);
#endif
