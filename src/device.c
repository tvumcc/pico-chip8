#include "pico/stdlib.h"
#include "ST7735_TFT.h"

#include "device.h"
#include "roms.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

Display display;
CHIP8 chip8;
SCHIP8 schip8;
PicoCHIP8 device;

void home_goto(PicoCHIP8* device) {
	fillScreen(ST7735_BLACK);
	drawText(3, 4, "PicoCHIP-8", ST7735_WHITE, ST7735_BLACK, 2);
	drawText(16, 20, "written by piigle", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 40, "CHIP-8", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 50, "Super CHIP-8", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 60, "XO-CHIP", ST7735_WHITE, ST7735_BLACK, 1);
	drawChar(20, 40, '>', ST7735_WHITE, ST7735_BLACK, 1);
	device->state = STATE_HOME;
	device->system_selection = 0;
}

void home_process_buttons(PicoCHIP8* device) {
	static unsigned int debounce_timer = 0;
	static unsigned int debounce = 250;

	// Move cursor up
	if (device->key_state[KEY_5] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 40 + device->system_selection * 10, 5, 7, ST7735_BLACK);
		device->system_selection = device->system_selection == 0 ? 2 : device->system_selection - 1;
		drawChar(20, 40 + device->system_selection * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Move cursor down
	} else if (device->key_state[KEY_8] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 40 + device->system_selection * 10, 5, 7, ST7735_BLACK);
		device->system_selection = (device->system_selection + 1) % 3;
		drawChar(20, 40 + device->system_selection * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Select
	} else if (device->key_state[KEY_9]) {
		device->state = STATE_ROM_SELECT;
		device->system = device->system_selection;
		device->rom_selection = 0;
		device->page = 0;
		rom_select_goto(device);
		sleep_ms(500); // Prevents from instantly running the first rom if you don't immediately let go of the button
	}
}

void rom_select_goto(PicoCHIP8* device) {
	fillScreen(ST7735_BLACK);
	switch (device->system) {
		case CHIP_8:
			drawText(3, 4, "CHIP-8", ST7735_WHITE, ST7735_BLACK, 2);
			for (int i = device->page * 8; i < min(sizeof(chip8_roms) / sizeof(Program), (device->page+1) * 8); i++) {
				drawText(32, 30 + (10 * (i % 8)), chip8_roms[i].name, ST7735_WHITE, ST7735_BLACK, 1);
			}
			drawChar(20, 30 + (10 * (device->rom_selection % 8)), '>', ST7735_WHITE, ST7735_BLACK, 1);
			break;
		case SuperCHIP_8:
			drawText(3, 4, "SuperCH8", ST7735_WHITE, ST7735_BLACK, 2);
			for (int i = device->page * 8; i < min(sizeof(schip8_roms) / sizeof(Program), (device->page+1) * 8); i++) {
				drawText(32, 30 + (10 * (i % 8)), schip8_roms[i].name, ST7735_WHITE, ST7735_BLACK, 1);
			}
			drawChar(20, 30 + (10 * (device->rom_selection % 8)), '>', ST7735_WHITE, ST7735_BLACK, 1);
			break;
		case XO_CHIP:
			drawText(3, 4, "XO-CHIP", ST7735_WHITE, ST7735_BLACK, 2);
			drawText(16, 20, "Coming Soon!", ST7735_WHITE, ST7735_BLACK, 1);
			break;
	}
}

void rom_select_process_buttons(PicoCHIP8* device) {
	static unsigned int debounce_timer = 0;
	static unsigned int debounce = 250;
	if (debounce_timer == 0) debounce_timer = to_ms_since_boot(get_absolute_time());

	int num_roms = 0;
	switch (device->system) {
		case CHIP_8:
			num_roms = sizeof(chip8_roms) / sizeof(Program);
			break;
		case SuperCHIP_8:
			num_roms = sizeof(schip8_roms) / sizeof(Program);
			break;
		case XO_CHIP:
			break;
	}

	// Move cursor up
	if (device->key_state[KEY_5] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 30 + (device->rom_selection % 8) * 10, 5, 7, ST7735_BLACK);
		device->rom_selection = device->rom_selection == 0 ? num_roms-1 : device->rom_selection - 1;
		if (device->rom_selection / 8 != device->page) {
			device->page = device->rom_selection / 8;
			rom_select_goto(device);
		}
		drawChar(20, 30 + (device->rom_selection % 8) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
		// Move cursor down
	} else if (device->key_state[KEY_8] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 30 + (device->rom_selection % 8) * 10, 5, 7, ST7735_BLACK);
		device->rom_selection = (device->rom_selection + 1) % (num_roms-1);
		if (device->rom_selection / 8 != device->page) {
			device->page = device->rom_selection / 8;
			rom_select_goto(device);
		}
		drawChar(20, 30 + (device->rom_selection % 8) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
		// Select
	} else if (device->key_state[KEY_9] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		device->state = STATE_GAME;
		display = display_init();
		switch (device->system) {
			case CHIP_8:
				chip8 = chip8_init(chip8_roms[device->rom_selection].data, chip8_roms[device->rom_selection].size);
				break;
			case SuperCHIP_8:
				schip8 = schip8_init(schip8_roms[device->rom_selection].data, schip8_roms[device->rom_selection].size);
				break;
			default:
				break;
		}
		debounce_timer = to_ms_since_boot(get_absolute_time());
	}
}
