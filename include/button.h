#ifndef BUTTON_H
#define BUTTON_H

// Row 1
#define KEY_1 0
#define KEY_2 1
#define KEY_3 2
#define KEY_C 3
// Row 2
#define KEY_4 4
#define KEY_5 5
#define KEY_6 6
#define KEY_D 7
// Row 3
#define KEY_7 8
#define KEY_8 9
#define KEY_9 10
#define KEY_E 11
// Row 4
#define KEY_A 12
#define KEY_0 13
#define KEY_B 14
#define KEY_F 15

// GPIO pins for shift registers
#define SHIFT_LOAD 20
#define SHIFT_CLK 21
#define SHIFT_IN 22

extern int keys[16];

// Initializes the GPIO pins of the shift registers and the reset button
void buttons_init();

// Loads in the inputs from the shift register into memory
void process_buttons(unsigned char* out);
#endif
