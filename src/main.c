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

// Each button will be mapped to a single GPIO pin for now
// In the future, shift registers will be used
#define KEY_2 20 // UP
#define KEY_4 18 // LEFT
#define KEY_6 21 // RIGHT
#define KEY_8 19 // DOWN


void process_buttons(unsigned char* out, uint8_t* keys);

int main() {
	uint8_t keys[16] = {0};
	keys[2] = KEY_2;
	keys[4] = KEY_4;
	keys[6] = KEY_6;
	keys[8] = KEY_8;
	for (int i = 0; i < 16; i++) {
		if (keys[i] != 0) {
			gpio_init(keys[i]);
			gpio_set_dir(keys[i], GPIO_IN);
			gpio_pull_down(keys[i]);
		}
	}

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
	CHIP8 chip8 = chip8_init(octojam2_program, sizeof(octojam2_program));

	// Keep program running
	while (1) {
		unsigned int current_time = to_ms_since_boot(get_absolute_time());
		if (current_time > last_frame + clock_speed) {
			process_buttons(chip8.keys, keys);
			tick(&chip8, &display);
			last_frame = current_time;

			if (chip8.keys[2]) gpio_put(25, 0);
			else gpio_put(25, 1);
		}
	}
	return 0;
}

void process_buttons(unsigned char* out, uint8_t* keys) {
	for (int i = 0; i < 16; i++) {
		if (keys[i] != 0) {
			out[i] = gpio_get(keys[i]);
		}
	}
}
