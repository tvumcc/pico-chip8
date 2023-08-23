#include "pico/stdlib.h"
#include "ST7735_TFT.h"

#include "device.h"
#include "roms.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

PicoCHIP8 device;
Display display;
CHIP8 chip8;

void rom_select_goto(PicoCHIP8* device) {
	device->state = STATE_ROM_SELECT;
	fillScreen(ST7735_BLACK);
	drawText(3, 4, "PicoCHIP-8", ST7735_WHITE, ST7735_BLACK, 2);
	for (int i = device->page * 8; i < min(sizeof(chip8_roms) / sizeof(Program), (device->page+1) * 8); i++) {
		drawText(32, 30 + (10 * (i % 8)), chip8_roms[i].name, ST7735_WHITE, ST7735_BLACK, 1);
	}
	drawChar(20, 30 + (10 * (device->rom_selection % 8)), '>', ST7735_WHITE, ST7735_BLACK, 1);
}

void rom_select_process_buttons(PicoCHIP8* device) {
	static unsigned int debounce_timer = 0;
	static unsigned int debounce = 250;
	if (debounce_timer == 0) debounce_timer = to_ms_since_boot(get_absolute_time());

	int num_roms = sizeof(chip8_roms) / sizeof(Program);

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
		device->rom_selection = (device->rom_selection + 1) % (num_roms);
		if (device->rom_selection / 8 != device->page) {
			device->page = device->rom_selection / 8;
			rom_select_goto(device);
		}
		drawChar(20, 30 + (device->rom_selection % 8) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Select
	} else if (device->key_state[KEY_9] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		device->state = STATE_GAME;
		display = display_init(ST7735_BLACK, ST7735_WHITE);
		chip8_init(&chip8, chip8_roms[device->rom_selection].data, chip8_roms[device->rom_selection].size);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	}
}
