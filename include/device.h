#ifndef DEVICE_H
#define DEVICE_H

#include "chip-8/display.h"
#include "chip-8/chip8.h"
#include "system.h"

#define KEY_5 20 // UP
#define KEY_7 18 // LEFT
#define KEY_9 21 // RIGHT
#define KEY_8 19 // DOWN

#define KEY_RESET 2 // Reset button is at GPIO pin 2

#define STATE_HOME 0
#define STATE_ROM_SELECT 1
#define STATE_GAME 2

extern Display display;
extern CHIP8 chip8;

typedef struct PicoCHIP8 {
	uint8_t key_state[18]; // 16 and 17 are for RESET and SELECT	
	uint8_t state;
	System system;

	// Menu Stuff
	uint8_t system_selection;
	uint8_t rom_selection;
	uint8_t page;
} PicoCHIP8;

void home_goto(PicoCHIP8* device);
void home_process_buttons(PicoCHIP8* device);
void rom_select_goto(PicoCHIP8* device);
void rom_select_process_buttons(PicoCHIP8* device);

#endif
