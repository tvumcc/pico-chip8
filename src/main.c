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


void go_to_home();
void process_buttons(unsigned char* out, uint8_t* keys);

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

	gpio_init(25);
	gpio_set_dir(25, GPIO_OUT);

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

	Display display = display_init();
	CHIP8 chip8 = chip8_init(flightrunner_program, sizeof(flightrunner_program));

	uint8_t state = STATE_GAME;
	uint8_t home_screen_selection = 0;
	unsigned int debounce_timer = to_ms_since_boot(get_absolute_time());
	unsigned int debounce = 250;

	// Keep program running
	while (1) {
		// Reset logic
		gpio_put(25, gpio_get(KEY_RESET));
		if (gpio_get(KEY_RESET)) {
			if (gpio_get(KEY_7) && state != STATE_HOME) { // DOWN
				state = STATE_HOME;
				go_to_home();
			} else if (gpio_get(KEY_5) && state != STATE_GAME) { // UP
				state = STATE_GAME;
				chip8 = chip8_init(flightrunner_program, sizeof(flightrunner_program));
				display = display_init();
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
				if (gpio_get(KEY_5) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
					fillRect(20, 40 + (home_screen_selection) * 10, 5, 7, ST7735_BLACK);
					home_screen_selection = home_screen_selection == 0 ? 2 : home_screen_selection - 1;
					drawChar(20, 40 + (home_screen_selection) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
					debounce_timer = to_ms_since_boot(get_absolute_time());
				} else if (gpio_get(KEY_8) && (to_ms_since_boot(get_absolute_time()) > debounce_timer + debounce)) {
					fillRect(20, 40 + (home_screen_selection) * 10, 5, 7, ST7735_BLACK);
					home_screen_selection = (home_screen_selection + 1) % 3;
					drawChar(20, 40 + (home_screen_selection) * 10, '>', ST7735_WHITE, ST7735_BLACK, 1);
					debounce_timer = to_ms_since_boot(get_absolute_time());
				}
			}
		}
	}
	return 0;
}

void go_to_home() {
	fillScreen(ST7735_BLACK);
	drawText(3, 4, "PicoCHIP-8", ST7735_WHITE, ST7735_BLACK, 2);
	drawText(16, 20, "written by piigle", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 40, "CHIP-8", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 50, "Super CHIP-8", ST7735_WHITE, ST7735_BLACK, 1);
	drawText(32, 60, "XO-CHIP", ST7735_WHITE, ST7735_BLACK, 1);
	drawChar(20, 40, '>', ST7735_WHITE, ST7735_BLACK, 1);
}

void process_buttons(unsigned char* out, uint8_t* keys) {
	for (int i = 0; i < 16; i++) {
		if (keys[i] != 0) {
			out[i] = gpio_get(keys[i]);
		}
	}
}
