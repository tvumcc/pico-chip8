#include "super-chip-8/schip8.h"
#include "super-chip-8/schip8_opcodes.h"

SCHIP8 schip8_init(u8* program, size_t program_size) {
	SCHIP8 schip8;
	schip8.chip8 = chip8_init(program, program_size);

	// Big hex font
	for (int k = 0, i = 80; k < 80; i+=2, k++) {
		u8 out = 0;
		for (int j = 0; j < 4; j++) {
			out |= ((schip8.chip8.memory[k] & (1 << (j+4))) ? (3 << (j * 2)) : (0));
		}
		schip8.chip8.memory[i] = out;
		schip8.chip8.memory[i+1] = out;
	}

	// Initialize flag registers
	for (int i = 0; i < 16; i++) {
		schip8.flags[i] = 0;
	}

	return schip8;
}

void schip8_tick(SCHIP8* schip8, Display* display) {
	if (schip8->chip8.sound_timer > 0)	
		schip8->chip8.sound_timer--;
	if (schip8->chip8.delay_timer > 0)
		schip8->chip8.delay_timer--;
	
	u16 instruction = fetch_instruction(&schip8->chip8);
	schip8_decode_and_execute(schip8, display, instruction);
}

void schip8_decode_and_execute(SCHIP8* schip8, Display* display, u16 instruction) {
	bool is_chip8_instruction = decode_and_execute(&schip8->chip8, display, instruction);
	if (!is_chip8_instruction || ((instruction & 0xF000) >> 12 == 0xD && (instruction & 0xF) == 0x0)) {
		u8 X   = (instruction & 0x0F00) >> 8;
		u8 Y   = (instruction & 0x00F0) >> 4;
		u8 N   = (instruction & 0x000F);

		switch((instruction & 0xF000) >> 12) {
			case 0x00:
				switch (instruction & 0x00FF) {
					case 0xFF:
						op_00FF(schip8, display);
						break;
					case 0xFE:
						op_00FE(schip8, display);
						break;
					case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
					case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
						op_00CN(schip8, display, N);
						break;
					case 0xFB:
						op_00FB(schip8, display);
						break;
					case 0xFC:
						op_00FC(schip8, display);
						break;
					case 0xFD:
						op_00FD(schip8);
						break;
				}
				break;
			case 0x0D:
				op_DXY0(schip8, display, X, Y);
				break;
			case 0x0F:
				switch (instruction & 0xFF) {
					case 0x30:
						op_FX30(schip8, X);
						break;
					case 0x75:
						op_FX75(schip8, X);
						break;
					case 0x85:
						op_FX85(schip8, X);
						break;
				}
				break;
		}
	}
}
