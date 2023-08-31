#ifndef DEVICE_H
#define DEVICE_H
#include "display.h"
#include "button.h"

#include "chip-8/chip8.h"

#define STATE_ROM_SELECT 1
#define STATE_GAME 2

typedef struct PicoCHIP8 {
	u8 key_state[16]; // button 16 is for RESET
	u8 state;

	// Menu Stuff
	u8 rom_selection;
	u8 page;
} PicoCHIP8;

extern PicoCHIP8 device;
extern Display display;
extern CHIP8 chip8;

void rom_select_goto(PicoCHIP8* device);
void rom_select_process_buttons(PicoCHIP8* device);
#endif
