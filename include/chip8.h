#pragma once
#include <stdint.h>

namespace Chippy
{
	class Chip8
	{
	public:
		enum EmulationMode
		{
			MODE_CHIP8,
			MODE_CHIP48
		};

		uint32_t	n_current_mode;
		uint16_t	n_program_counter;
		uint16_t	n_i_register;
		uint8_t		n_stack_pointer;
		uint8_t		n_register[16];
		uint8_t		n_memory[0x1000];

		Chip8();
		void reset();
		void setmode(uint32_t mode);
		bool loadmem(const char* filename);
		bool step();
		void disassemble(char* str, uint16_t addr);
	};
}