#include "display_chip8.h"
#include "olcPixelGameEngine.h"

Chippy::DisplayChip8::DisplayChip8() {
	n_display_width		= DISPLAY_WIDTH;
	n_display_height	= DISPLAY_HEIGHT;
	n_display_scalar	= DISPLAY_SCALAR;
	n_display_buffer_len	= DISPLAY_WIDTH * DISPLAY_HEIGHT / 8;
	n_display_buffer_data[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]
				= { (uint8_t) 0xFF };
}

void Chippy::DisplayChip8::init() {
	for (int i = 0; i < this->n_display_buffer_len; i++) {
		// Set up a checkerboard pattern.
		n_display_buffer_data[i] = (i / (n_display_width / 8)) % 2 ? 0b10101010 : 0b01010101;
	}
}