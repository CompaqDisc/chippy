#pragma once
#include <stdint.h>
#include <cstdio>
#define DISPLAY_WIDTH	64
#define DISPLAY_HEIGHT	32
#define DISPLAY_SCALAR	8

namespace Chippy {
	class DisplayChip8 {
	public:
		uint8_t		n_display_width;
		uint8_t		n_display_height;
		uint8_t		n_display_scalar;
		uint16_t	n_display_buffer_len;
		uint8_t		n_display_buffer_data[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

		DisplayChip8();
		void init();
	};
}