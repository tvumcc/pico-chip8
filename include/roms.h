#ifndef ROMS_H
#define ROMS_H
#include <stdlib.h>

typedef struct Program {
	char name[14];
	unsigned char* data;
	size_t size;
} Program;

extern Program chip8_roms[10];

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
#endif
