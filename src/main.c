#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hw.h"
#include "ST7735_TFT.h"

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

	gpio_init(25); // On-board LED
	gpio_set_dir(25, GPIO_OUT);

	// Do some drawing
	TFT_BlackTab_Initialize();
	fillScreen(ST7735_BLUE); // why is blue red and red blue...


	// Keep program running
	while (1) {
		gpio_put(25, 1);
		sleep_ms(1000);
		gpio_put(25, 0);
		sleep_ms(1000);
	}
	return 0;
}
