#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "chip-8/display.h"
#include "chip-8/chip8.h"
#include "roms.h"

// The following are exact redefinitions that are already defined in the pico-st7735 CMakeLists.txt
// If the hardware configuration needs to be changed, these can be overriden
#define SPI_TFT_PORT spi1 // The display will run using SPI1 (Pico has SPI0 and SPI1)
#define SPI_TFT_CS 9 // Chip Select
#define SPI_TFT_DC 10 // Data / Command
#define SPI_TFT_RST 11 // Reset
// End of redefinitions

#define SPI_PORT spi1
#define SPI_RX 12
#define SPI_SCK 14
#define SPI_TX 15
#define TFT_ENABLE_BLACK
#define TFT_ENABLE_TEXT


// Each button will be mapped to a single GPIO pin for now
// In the future, shift registers will be used
#define KEY_5 20 // UP
#define KEY_7 18 // LEFT
#define KEY_9 21 // RIGHT
#define KEY_8 19 // DOWN
#define KEY_RESET 2
#define KEY_SELECT 3

#define STATE_HOME 0
#define STATE_ROM_SELECT 1
#define STATE_GAME 2

typedef enum System {
	CHIP_8,
	SuperCHIP_8,
	XO_CHIP,
} System;

typedef struct Program {
	char name[20];
	unsigned char* data;
	size_t size;
} Program;

typedef struct PicoCHIP8 {
	uint8_t key_state[18]; // 16 and 17 are for RESET and SELECT	
	uint8_t state;
	System system;

	// Menu Stuff
	uint8_t system_selection;
	uint8_t rom_selection;
} PicoCHIP8;

void home_goto(PicoCHIP8* device);
void home_process_buttons(PicoCHIP8* device);
void rom_select_goto(PicoCHIP8* device);
void rom_select_process_buttons(PicoCHIP8* device);
void process_buttons(unsigned char* out, uint8_t* keys);

Program chip8_roms[] = {
	{"IBM Logo", ibm_program, sizeof(ibm_program)},
	{"Octojam2", octojam2_program, sizeof(octojam2_program)},
	{"Flightrunner", flightrunner_program, sizeof(flightrunner_program)},
	{"Snek", snek_program, sizeof(snek_program)},
	{"Br8kout", breakout_program, sizeof(breakout_program)},
	{"Pet Dog", petdog_program, sizeof(petdog_program)},
};

Display display;
CHIP8 chip8;

int main() {
	uint8_t keys[16] = {0};
	keys[5] = KEY_5;
	keys[7] = KEY_7;
	keys[9] = KEY_9;
	keys[8] = KEY_8;
	keys[16] = KEY_RESET;
	for (int i = 0; i < 16; i++) {
		if (keys[i] != 0) {
			gpio_init(keys[i]);
			gpio_set_dir(keys[i], GPIO_IN);
			gpio_pull_down(keys[i]);
		}
	}

	// Display Stuff
	tft_height = (uint8_t)131;
	tft_width = (uint8_t)130;
	stdio_init_all();
	spi_init(SPI_PORT, 60000000); // 60 Mhz, SPI Baud Rate
	gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
	gpio_set_function(SPI_SCK,GPIO_FUNC_SPI);
	gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
	tft_spi_init();
	TFT_BlackTab_Initialize();
	fillScreen(ST7735_BLACK); 


	unsigned int last_frame = to_ms_since_boot(get_absolute_time());
	unsigned int clock_speed = 1;
	srand(last_frame); 

	PicoCHIP8 device;
	device.state = STATE_HOME;
	device.system_selection = 0;
	device.rom_selection = 0;
	device.system = CHIP_8;
	home_goto(&device);

	while (true) {
		process_buttons(device.key_state, keys);

		if (device.key_state[16]) {
			if (device.key_state[7] && device.state != STATE_HOME) {
				home_goto(&device);
			}
		} else {
			if (device.state == STATE_GAME) {
				unsigned int current_time = to_ms_since_boot(get_absolute_time());
				if (current_time > last_frame + clock_speed) {
					for (int i = 0; i < 16; i++) chip8.keys[i] = device.key_state[i];
					tick(&chip8, &display);
					last_frame = current_time;
				}
			} else if (device.state == STATE_HOME) {
				home_process_buttons(&device);
			} else if (device.state == STATE_ROM_SELECT) {
				rom_select_process_buttons(&device);
			}
		}
	}
}

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
	if (device->key_state[5] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 40 + device->system_selection * 10, 5, 7, ST7735_BLACK);
		device->system_selection = device->system_selection == 0 ? 2 : device->system_selection - 1;
		drawChar(20, 40 + device->system_selection * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Move cursor down
	} else if (device->key_state[8] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 40 + device->system_selection * 10, 5, 7, ST7735_BLACK);
		device->system_selection = (device->system_selection + 1) % 3;
		drawChar(20, 40 + device->system_selection * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Select
	} else if (device->key_state[9]) {
		device->state = STATE_ROM_SELECT;
		device->system = device->system_selection;
		rom_select_goto(device);
	}
}

void rom_select_goto(PicoCHIP8* device) {
	fillScreen(ST7735_BLACK);
	switch (device->system) {
		case CHIP_8:
			drawText(3, 4, "CHIP-8", ST7735_WHITE, ST7735_BLACK, 2);
			for (int i = 0; i < sizeof(chip8_roms) / sizeof(Program); i++) {
				drawText(32, 30 + (10 * i), chip8_roms[i].name, ST7735_WHITE, ST7735_BLACK, 1);
			}
			drawChar(20, 30, '>', ST7735_WHITE, ST7735_BLACK, 1);
			break;
		case SuperCHIP_8:
			drawText(3, 4, "SuperCH8", ST7735_WHITE, ST7735_BLACK, 2);
			drawText(16, 20, "Coming Soon!", ST7735_WHITE, ST7735_BLACK, 1);
			break;
		case XO_CHIP:
			drawText(3, 4, "XO-CHIP", ST7735_WHITE, ST7735_BLACK, 2);
			drawText(16, 20, "Coming Soon!", ST7735_WHITE, ST7735_BLACK, 1);
			break;
	}

	device->rom_selection = 0;
	sleep_ms(500); // Prevents from instantly running the first rom if you don't immediately let go of the button
}

void rom_select_process_buttons(PicoCHIP8* device) {
	static unsigned int debounce_timer = 0;
	static unsigned int debounce = 250;
	if (debounce_timer == 0) debounce_timer = to_ms_since_boot(get_absolute_time());

	switch (device->system) {
		case CHIP_8:
			// Move cursor up
			if (device->key_state[5] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
				fillRect(20, 30 + device->rom_selection * 10, 5, 7, ST7735_BLACK);
				device->rom_selection = device->rom_selection == 0 ? ((sizeof(chip8_roms) / sizeof(Program))-1) : device->rom_selection - 1;
				drawChar(20, 30 + device->rom_selection * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
				debounce_timer = to_ms_since_boot(get_absolute_time());
			// Move cursor down
			} else if (device->key_state[8] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
				fillRect(20, 30 + device->rom_selection * 10, 5, 7, ST7735_BLACK);
				device->rom_selection = (device->rom_selection + 1) % (sizeof(chip8_roms) / sizeof(Program));
				drawChar(20, 30 + device->rom_selection * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
				debounce_timer = to_ms_since_boot(get_absolute_time());
			// Select
			} else if (device->key_state[9] && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
				device->state = STATE_GAME;
				display = display_init();
				chip8 = chip8_init(chip8_roms[device->rom_selection].data, chip8_roms[device->rom_selection].size);
				debounce_timer = to_ms_since_boot(get_absolute_time());
			}
			break;
	}

}

void process_buttons(unsigned char* out, uint8_t* keys) {
	for (int i = 0; i < 18; i++) {
		if (keys[i] != 0) {
			out[i] = gpio_get(keys[i]);
		}
	}
}
