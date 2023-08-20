#ifndef DEVICE_H
#define DEVICE_H

#include "display.h"
#include "system.h"
#include "button.h"

#include "chip-8/chip8.h"

#define STATE_HOME 0
#define STATE_ROM_SELECT 1
#define STATE_GAME 2

typedef struct PicoCHIP8 {
	u8 key_state[18]; // 16 and 17 are for RESET and SELECT	
	u8 state;
	System system;

	// Menu Stuff
	u8 system_selection;
	u8 rom_selection;
	u8 page;
} PicoCHIP8;

extern PicoCHIP8 device;
extern Display display;
extern CHIP8 chip8;

void home_goto(PicoCHIP8* device);
void home_process_buttons(PicoCHIP8* device);
void rom_select_goto(PicoCHIP8* device);
void rom_select_process_buttons(PicoCHIP8* device);
#endif
