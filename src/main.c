#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "chip-8/display.h"
#include "chip-8/chip8.h"
#include "roms.h"
#include "system.h"
#include "device.h"

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

#define SHIFT_LOAD 20
#define SHIFT_CLK 21
#define SHIFT_IN 22

void process_buttons(unsigned char* out);

int main() {
	gpio_init(KEY_RESET);
	gpio_set_dir(KEY_RESET, GPIO_IN);
	gpio_pull_down(KEY_RESET);

	gpio_init(SHIFT_LOAD);
	gpio_set_dir(SHIFT_LOAD, GPIO_OUT);
	gpio_put(SHIFT_LOAD, 1);
	gpio_init(SHIFT_CLK);
	gpio_set_dir(SHIFT_CLK, GPIO_OUT);
	gpio_put(SHIFT_CLK, 0);
	gpio_init(SHIFT_IN);
	gpio_set_dir(SHIFT_IN, GPIO_IN);

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
	device.page = 0;
	device.system = CHIP_8;
	home_goto(&device);

	while (true) {
		process_buttons(device.key_state);

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


void process_buttons(unsigned char* out) {
	sleep_ms(100);
	gpio_put(SHIFT_LOAD, 1);
	out[16] = gpio_get(KEY_RESET);
	char keys[17] = "";
	for (int i = 0; i < 16; i++) {
		out[i] = gpio_get(SHIFT_IN);
		keys[i] = out[i];
		gpio_put(SHIFT_CLK, 1);		
		sleep_us(1);
		gpio_put(SHIFT_CLK, 0);
		sleep_us(1);
	}
	drawText(20, 80, keys, ST7735_WHITE, ST7735_BLACK, 1);
	keys[16] = '\0';
	gpio_put(SHIFT_LOAD, 0);
}
