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

#define KEY_RESET 2

// Each button will be mapped to a single GPIO pin for now
// In the future, shift registers will be used
#define KEY_5 20 // UP
#define KEY_7 18 // LEFT
#define KEY_9 21 // RIGHT
#define KEY_8 19 // DOWN

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

void home_goto();
void home_process_buttons(uint8_t* state, uint8_t* selection, uint8_t* rom_selection, System* system, unsigned int up, unsigned int down, unsigned int enter);
void rom_select_goto(System system);
void rom_select_process_buttons(System system, uint8_t* state, uint8_t* rom_selection, unsigned int up, unsigned int down, unsigned int enter);
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
	for (int i = 0; i < 16; i++) {
		if (keys[i] != 0) {
			gpio_init(keys[i]);
			gpio_set_dir(keys[i], GPIO_IN);
			gpio_pull_down(keys[i]);
		}
	}

	gpio_init(KEY_RESET);
	gpio_set_dir(KEY_RESET, GPIO_IN);
	gpio_pull_down(KEY_RESET);

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

	uint8_t state = STATE_HOME;
	uint8_t home_screen_selection = 0;
	uint8_t rom_selection = 0;
	System system = CHIP_8;

	home_goto();

	// Keep program running
	while (1) {
		// Reset logic
		if (gpio_get(KEY_RESET)) {
			if (gpio_get(KEY_7) && state != STATE_HOME) { // DOWN
				state = STATE_HOME;
				home_goto();
				home_screen_selection = 0;
			}
		} else {
			if (state == STATE_GAME) {
				unsigned int current_time = to_ms_since_boot(get_absolute_time());
				if (current_time > last_frame + clock_speed) {
					process_buttons(chip8.keys, keys);
					tick(&chip8, &display);
					last_frame = current_time;
				}
			} else if (state == STATE_HOME) {
				home_process_buttons(&state, &home_screen_selection, &rom_selection, &system, KEY_5, KEY_8, KEY_9);
			} else if (state == STATE_ROM_SELECT) {
				rom_select_process_buttons(system, &state, &rom_selection, KEY_5, KEY_8, KEY_9);
			}
		}
	}
	return 0;
}

void home_goto() {
	fillScreen(ST7735_BLACK);
	drawText(3, 4, "PicoCHIP-8", ST7735_WHITE, ST7735_BLACK, 2);
	drawText(16, 20, "written by piigle", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 40, "CHIP-8", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 50, "Super CHIP-8", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 60, "XO-CHIP", ST7735_WHITE, ST7735_BLACK, 1);
	drawChar(20, 40, '>', ST7735_WHITE, ST7735_BLACK, 1);
}

void home_process_buttons(uint8_t* state, uint8_t* selection, uint8_t* rom_selection, System* system, unsigned int up, unsigned int down, unsigned int enter) {
	static unsigned int debounce_timer = 0;
	static unsigned int debounce = 250;

	// Move cursor up
	if (gpio_get(up) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 40 + (*selection) * 10, 5, 7, ST7735_BLACK);
		*selection = *selection == 0 ? 2 : *selection- 1;
		drawChar(20, 40 + (*selection) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Move cursor down
	} else if (gpio_get(down) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
		fillRect(20, 40 + (*selection) * 10, 5, 7, ST7735_BLACK);
		*selection = (*selection + 1) % 3;
		drawChar(20, 40 + (*selection) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
		debounce_timer = to_ms_since_boot(get_absolute_time());
	// Select
	} else if (gpio_get(enter)) {
		*state = STATE_ROM_SELECT;
		*system = *selection;
		*rom_selection = 0;
		rom_select_goto(*selection);
	}
}

void rom_select_goto(System system) {
	fillScreen(ST7735_BLACK);
	switch (system) {
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
	sleep_ms(500);
}

void rom_select_process_buttons(System system, uint8_t* state, uint8_t* rom_selection, unsigned int up, unsigned int down, unsigned int enter) {
	static unsigned int debounce_timer = 0;
	static unsigned int debounce = 250;
	if (debounce_timer == 0) debounce_timer = to_ms_since_boot(get_absolute_time());

	switch (system) {
		case CHIP_8:
			// Move cursor up
			if (gpio_get(up) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
				fillRect(20, 30 + (*rom_selection) * 10, 5, 7, ST7735_BLACK);
				*rom_selection = *rom_selection == 0 ? ((sizeof(chip8_roms) / sizeof(Program))-1) : *rom_selection - 1;
				drawChar(20, 30 + (*rom_selection) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
				debounce_timer = to_ms_since_boot(get_absolute_time());
			// Move cursor down
			} else if (gpio_get(down) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
				fillRect(20, 30 + (*rom_selection) * 10, 5, 7, ST7735_BLACK);
				*rom_selection = (*rom_selection + 1) % (sizeof(chip8_roms) / sizeof(Program));
				drawChar(20, 30 + (*rom_selection) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
				debounce_timer = to_ms_since_boot(get_absolute_time());
			// Select
			} else if (gpio_get(enter) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
				*state = STATE_GAME;
				display = display_init();
				chip8 = chip8_init(chip8_roms[*rom_selection].data, chip8_roms[*rom_selection].size);
				debounce_timer = to_ms_since_boot(get_absolute_time());
			}
			break;
	}

}

void process_buttons(unsigned char* out, uint8_t* keys) {
	for (int i = 0; i < 16; i++) {
		if (keys[i] != 0) {
			out[i] = gpio_get(keys[i]);
		}
	}
}
