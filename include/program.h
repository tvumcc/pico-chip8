#ifndef PROGRAM_H
#define PROGRAM_H
#include "pico/stdlib.h"
#include <stdint.h>

typedef struct Program {
	char name[17];
	uint8_t* data;
	size_t size;
} Program;
#endif
