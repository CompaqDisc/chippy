#include <stdint.h>

namespace Chippy {
	class Chip8 {
		uint16_t	n_program_counter	= 0x0000;
		uint8_t		n_stack_pointer		= 0x00;
		uint8_t		n_register[16]		= { 0x00 };

		Chip8() {
			this->reset();
		}

		void reset() {
			this->n_program_counter = 0x0000;
		}
	};
}