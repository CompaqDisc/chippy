#include <stdint.h>
#include <cstdio>
#include "chip8.h"

Chippy::Chip8::Chip8() {
	this->reset();
}

void Chippy::Chip8::reset() {
	this->n_program_counter = 0x200;
	for (uint16_t i = 0x000; i < 0x1000; i++)
		this->n_memory[i] = i ^ (i >> 8) & 0xff;
}

void Chippy::Chip8::disassemble(char* str, uint16_t addr) {
	switch (this->n_memory[addr] >> 4) {
	case 0x0:
		if (this->n_memory[addr + 1] == 0xe0)
			sprintf(str, "%04X: CLS", addr);
		else if (this->n_memory[addr + 1] == 0xee)
			sprintf(str, "%04X: RET", addr);
		else
		{
			sprintf(
				str, "%04X: SYS %03X", addr,
				(uint16_t) ((this->n_memory[addr] & 0xf) << 8 | this->n_memory[addr + 1])
			);
		}
		break;
	default:
		sprintf(str, "%04X: ILLEGAL", addr);
		break;
	}
}