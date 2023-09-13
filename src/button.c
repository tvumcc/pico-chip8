#include <pico/stdlib.h>

#include "button.h"

int keys[16] = {KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F};

void buttons_init() {
	gpio_init(SHIFT_LOAD);
	gpio_set_dir(SHIFT_LOAD, GPIO_OUT);
	gpio_put(SHIFT_LOAD, 1);

	gpio_init(SHIFT_CLK);
	gpio_set_dir(SHIFT_CLK, GPIO_OUT);
	gpio_put(SHIFT_CLK, 0);

	gpio_init(SHIFT_IN);
	gpio_set_dir(SHIFT_IN, GPIO_IN);
}

void process_buttons(uint8_t* out) {
	gpio_put(SHIFT_LOAD, 1);
	for (int i = 0; i < 16; i++) {
		out[i] = gpio_get(SHIFT_IN);
		gpio_put(SHIFT_CLK, 1);		
		sleep_us(1);
		gpio_put(SHIFT_CLK, 0);
		sleep_us(1);
	}
	gpio_put(SHIFT_LOAD, 0);
}
