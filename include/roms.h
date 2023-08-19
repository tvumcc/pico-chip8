#ifndef ROMS_H
#define ROMS_H
#include <stdlib.h>

typedef struct Program {
	char name[16];
	unsigned char* data;
	size_t size;
} Program;

extern Program chip8_roms[14];
extern Program schip8_roms[3];

extern unsigned char ibm_program[132];
extern unsigned char octojam2_program[1240];
extern unsigned char flightrunner_program[295];
extern unsigned char snek_program[65];
extern unsigned char breakout_program[280];
extern unsigned char petdog_program[1869];
extern unsigned char merlin_program[345];
extern unsigned char tank_program[224];
extern unsigned char tetris_program[494];
extern unsigned char pong_program[246];
extern unsigned char corax_program[697];
extern unsigned char flags_program[1017];
extern unsigned char quirks_program[3232];
extern unsigned char keypad_program[913];

extern unsigned char sine_program[282];
extern unsigned char binding_program[3552];
extern unsigned char snake_program[1438];
#endif
