#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "roms.h"
#include "device.h"
#include "display.h"
#include "button.h"

#include "chip-8/chip8.h"

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

int main() {
	buttons_init();

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
	unsigned int clock_500hz = 2;
	unsigned int last_frame_60hz = to_ms_since_boot(get_absolute_time());
	unsigned int clock_60hz = 17;
	srand(last_frame); 

	device.state = STATE_ROM_SELECT;
	device.rom_selection = 0;
	device.page = 0;
	rom_select_goto(&device);

	while (true) {
		process_buttons(device.key_state);
		unsigned int current_time = to_ms_since_boot(get_absolute_time());

		if (device.key_state[KEY_F] && device.key_state[KEY_1] && device.state != STATE_ROM_SELECT) {
			rom_select_goto(&device);
		} else {
			switch (device.state) {
				case STATE_GAME:
					if (current_time > last_frame_60hz + clock_60hz) {
						timer_tick(&chip8);
						last_frame_60hz = current_time;
					}
					if (current_time > last_frame + clock_500hz) {
						for (int i = 0; i < 16; i++) chip8.keys[i] = device.key_state[keys[i]];
						tick(&chip8, &display);
						last_frame = current_time;
					}
					break;
				case STATE_ROM_SELECT:
					rom_select_process_buttons(&device);
					break;
			}
		}
	}
}
