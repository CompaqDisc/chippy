#include "display.h"
#include "olcPixelGameEngine.h"

Chippy::Display::Display()
{
	this->n_active_gfx_mode = MODE_CHIP8;
	this->init(this->n_active_gfx_mode);
}

void Chippy::Display::init(uint32_t gfxmode)
{
	if (gfxmode == MODE_CHIP8)
		this->init(64, 32, 8);
	if (gfxmode == MODE_CHIP48)
		this->init(128, 64, 4);
}

void Chippy::Display::init(uint32_t width, uint32_t height, uint32_t scale)
{
	n_display_width		= width;
	n_display_height	= height;
	n_display_scale		= scale;
	n_display_buffer_len	= width * height / 8;
	n_display_buffer_data	= new uint8_t[width * height / 8];

	for (int i = 0; i < this->n_display_buffer_len; i++)
	{
		// Set up a checkerboard pattern.
		n_display_buffer_data[i] = (i / (n_display_width / 8)) % 2 ? 0b10101010 : 0b01010101;
	}
}