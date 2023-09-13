#ifndef ROMS_H
#define ROMS_H
#include <stdlib.h>
#include <stdint.h>

typedef struct Program {
	char name[16];
	uint8_t* data;
	size_t size;
} Program;

extern Program chip8_roms[14];

extern uint8_t ibm_program[132];
extern uint8_t octojam2_program[1240];
extern uint8_t flightrunner_program[295];
extern uint8_t snek_program[65];
extern uint8_t breakout_program[280];
extern uint8_t petdog_program[1869];
extern uint8_t merlin_program[345];
extern uint8_t tank_program[224];
extern uint8_t tetris_program[494];
extern uint8_t pong_program[246];
extern uint8_t corax_program[697];
extern uint8_t flags_program[1017];
extern uint8_t quirks_program[3232];
extern uint8_t keypad_program[913];
#endif
