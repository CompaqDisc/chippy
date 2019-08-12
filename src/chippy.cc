#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "display_chip8.h"

#define CANVAS_OFFSET	4

namespace Chippy {
	class Chippy : public olc::PixelGameEngine {
	public:
		DisplayChip8 dis;
		
		Chippy() {
			sAppName = "Chippy";
		}

		bool OnUserCreate() override {
			dis.init();
			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override {
			Clear(olc::BLACK);

			/********
			 * Oh boy...
			 * 
			 * If ya couldn't tell, this takes the bitmapped array
			 * `n_display_buffer_data`, and loops over its bits,
			 * it then scales these 'pixels' by the scalar value onto the canvas.
			 */

			// But first, draw bounds of emulated screen.
			FillRect(
				0, 0,
				dis.n_display_width  * dis.n_display_scalar + CANVAS_OFFSET * 2,
				dis.n_display_height * dis.n_display_scalar + CANVAS_OFFSET * 2,
				olc::DARK_MAGENTA
			);

			for (int n_buffer_idx = 0; n_buffer_idx < dis.n_display_buffer_len; n_buffer_idx++) {
				for (int n_position = 0; n_position < 8; n_position++) {
					// Ensures most significant bit is leftmost.
					uint8_t mask = 1 << 7 - n_position;
					for (int n_offset_x = 0; n_offset_x < dis.n_display_scalar; n_offset_x++) {
						for (int n_offset_y = 0; n_offset_y < dis.n_display_scalar; n_offset_y++) {
							Draw(
								(n_buffer_idx * 8 + n_position) % 
									dis.n_display_width * dis.n_display_scalar + n_offset_x + CANVAS_OFFSET,
								(n_buffer_idx * 8 + n_position) /
									dis.n_display_width * dis.n_display_scalar + n_offset_y + CANVAS_OFFSET,
								(dis.n_display_buffer_data[n_buffer_idx] & mask) ? olc::WHITE : olc::BLACK
							);
						} // n_offset_y
					} // n_offset_x
				} // n_position
			} // n_buffer_idx

			return true;
		}
	};
}

int main(int argc, char** argv) {
	Chippy::Chippy emulator;
	if (emulator.Construct(854, 480, 1, 1, false))
		emulator.Start();

	return 0;
}