#ifndef DEVICE_H
#define DEVICE_H

#include "chip-8/display.h"
#include "chip-8/chip8.h"
#include "system.h"

// Row 1
#define KEY_1 0
#define KEY_2 1
#define KEY_3 2
#define KEY_C 3
// Row 2
#define KEY_4 4
#define KEY_5 5
#define KEY_6 6
#define KEY_D 7
// Row 3
#define KEY_7 8
#define KEY_8 9
#define KEY_9 10
#define KEY_E 11
// Row 4
#define KEY_A 12
#define KEY_0 13
#define KEY_B 14
#define KEY_F 15
#define KEY_RESET 16  // Reset button is at GPIO pin 2

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
