#include "display.h"
#include "olcPixelGameEngine.h"

Chippy::Display::Display() {}

void Chippy::Display::init(uint32_t width, uint32_t height, uint32_t scale) {
	n_display_width		= width;
	n_display_height	= height;
	n_display_scale		= scale;
	n_display_buffer_len	= width * height / 8;
	n_display_buffer_data	= new uint8_t[width * height / 8];

	for (int i = 0; i < this->n_display_buffer_len; i++) {
		// Set up a checkerboard pattern.
		n_display_buffer_data[i] = (i / (n_display_width / 8)) % 2 ? 0b10101010 : 0b01010101;
	}
}