#include "super-chip-8/schip8_opcodes.h"
#include "device.h"

void op_00FF(SCHIP8* schip8, Display* display) {
	schip8->extended_resolution = true;
}

void op_00FE(SCHIP8* schip8, Display* display) {
	schip8->extended_resolution = false;
}

void op_00CN(SCHIP8* schip8, Display* display) {

}

void op_00FB(SCHIP8* schip8, Display* display) {

}

void op_00FC(SCHIP8* schip8, Display* display) {

}

void op_00FD(SCHIP8* schip8, Device* device) {
	home_goto(device);	
}

void op_DXY0(SCHIP8* schip8, Display* display, u8 X, u8 Y) {

}

void op_FX30(SCHIP8* schip8, u8 X) {
	schip8->chip8.index_register = 0x50 + (10 * X);
}

void op_FX75(SCHIP8* schip8, u8 X) {
	for (int i = 0; i <= X; i++) {
		schip8.flags[i] = schip8->chip8.registers[i];
	}
}

void op_FX85(SCHIP8* schip8, u8 X) {
	for (int i = 0; i <= X; i++) {
		schip8->chip8.registers[i] = schip8.flags[i];
	}
}
