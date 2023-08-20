#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SUPER_CHIP8_WIDTH 128
#define SUPER_CHIP8_HEIGHT 64

typedef struct Display {
    unsigned char pixelArray[SUPER_CHIP8_HEIGHT][SUPER_CHIP8_WIDTH];
	unsigned char prev_buffer[SUPER_CHIP8_HEIGHT][SUPER_CHIP8_WIDTH];
	
	unsigned short bg_color;
	unsigned short bitplane0_color;
	unsigned short bitplane1_color;
	unsigned short blend_color;

	bool extended_resolution;
	unsigned char bitplane_mask;
} Display;

// Initialize the display array and colors
Display display_init(unsigned short bg, unsigned short bp0, unsigned short bp1, unsigned short blend);

// Draw the buffer to the screen
void display_draw(Display* display);

// Set pixel of display array
bool display_set_pixel(Display* display, int x, int y);

// Set all pixels of display array to 0x00 (black)
void display_clear(Display* display);

// Extends the display to 128x64, the new area will be completely empty
void display_hi_res(Display* display);

// Brings the display back to 64x32, cropping out anything that is outside of this range
void display_low_res(Display* display);
#endif
