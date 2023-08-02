#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

#include "chip-8/display.h"
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

int main() {
	// Adjust dimensions because the display is actually 131x130???
	tft_height = (uint8_t)131;
	tft_width = (uint8_t)130;

	// Initialize the hardware
	stdio_init_all();
	spi_init(SPI_PORT, 1000000); // 1 Mhz clock for SPI
	gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
	gpio_set_function(SPI_SCK,GPIO_FUNC_SPI);
	gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
	tft_spi_init();

	// Do some drawing
	TFT_BlackTab_Initialize();
	fillScreen(ST7735_BLUE); // why is blue red and red blue...

	unsigned char program[] = {
		0000, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 
		0x70, 0x09, 0xa2, 0x39, 0xd0, 0x1f, 0xa2, 0x48, 0x70, 0x08, 
		0xd0, 0x1f, 0x70, 0x04, 0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 
		0xa2, 0x66, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x75, 0xd0, 0x1f, 
		0x12, 0x28, 0xff, 0000, 0xff, 0000, 0x3c, 0000, 0x3c, 0000, 
		0x3c, 0000, 0x3c, 0000, 0xff, 0000, 0xff, 0xff, 0000, 0xff, 
		0000, 0x38, 0000, 0x3f, 0000, 0x3f, 0000, 0x38, 0000, 0xff, 
		0000, 0xff, 0x80, 0000, 0xe0, 0000, 0xe0, 0000, 0x80, 0000, 
		0x80, 0000, 0xe0, 0000, 0xe0, 0000, 0x80, 0xf8, 0000, 0xfc, 
		0000, 0x3e, 0000, 0x3f, 0000, 0x3b, 0000, 0x39, 0000, 0xf8, 
		0000, 0xf8, 0x03, 0000, 0x07, 0000, 0x0f, 0000, 0xbf, 0000, 
		0xfb, 0000, 0xf3, 0000, 0xe3, 0000, 0x43, 0xe0, 0000, 0xe0, 
		0000, 0x80, 0000, 0x80, 0000, 0x80, 0000, 0x80, 0000, 0xe0, 
		0000, 0xe0, 
	};

	// unsigned int last_frame = to_ms_since_boot(get_absolute_time());
	// unsigned int clock_speed = 1;

	// Seed the random number generator at the start time of the program
	srand((unsigned int)to_ms_since_boot(get_absolute_time())); 

	Display display = display_init();
	CHIP8 chip8 = chip8_init(program, sizeof(program));
	// Keep program running
	while (1) {
		//unsigned int current_time = to_ms_since_boot(get_absolute_time());
		//if (current_time > last_frame + clock_speed) {
			tick(&chip8, &display);
			//last_frame = current_time;
		//}
	}
	return 0;
}
