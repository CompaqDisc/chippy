#pragma once
#include <stdint.h>

namespace Chippy {
	class Display {
	public:
		enum GraphicsMode {
			MODE_CHIP8,
			MODE_CHIP48
		};

		uint8_t		n_display_width;
		uint8_t		n_display_height;
		uint8_t		n_display_scale;
		uint16_t	n_display_buffer_len;
		uint8_t*	n_display_buffer_data;
		uint32_t	n_active_gfx_mode;

		Display();
		void init(uint32_t width, uint32_t height, uint32_t scale);
		void clear();
	};
}