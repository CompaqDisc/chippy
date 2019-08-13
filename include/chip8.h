#pragma once
#include <stdint.h>

namespace Chippy {
	class Chip8 {
	public:
		uint16_t	n_program_counter;
		uint8_t		n_stack_pointer;
		uint8_t		n_register[16];
		uint8_t		n_memory[0xfff];

		Chip8();
		void reset();
		void disassemble(char* str, uint16_t addr);
	};
}