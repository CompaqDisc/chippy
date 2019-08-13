#define OLC_PGE_APPLICATION
#include <stdint.h>
#include "olcPixelGameEngine.h"
#include "display.h"
#include "chip8.h"

#define CANVAS_OFFSET	8

namespace Chippy {
	class Chippy : public olc::PixelGameEngine {
	public:
		enum EmulatorState {
			STATE_INIT,
			STATE_MENU,
			STATE_RUNNING,
			STATE_EXITING
		};

		Display*	dis	= new Display();
		Chip8*		chip	= new Chip8();
		uint16_t	n_hexview_base_addr	= 0x200;
		uint32_t	n_emu_state		= STATE_INIT;
		
		Chippy() {
			sAppName = "Chippy";
		}

		bool OnUserCreate() override {
			dis->init(64, 32, 8);
			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override {
			Clear(olc::VERY_DARK_GREY);

			if (GetKey(olc::Key::PGDN).bHeld) {
				if (!(n_hexview_base_addr >= (0xfff - 0x1d0)))
					n_hexview_base_addr += 8;
			}

			if (GetKey(olc::Key::PGUP).bHeld) {
				if (!(n_hexview_base_addr <= 0x000))
					n_hexview_base_addr -= 8;
			}

			if (GetKey(olc::Key::HOME).bPressed) {
				dis->init(64, 32, 8);
				n_emu_state = STATE_INIT;
			}

			if (GetKey(olc::Key::END).bPressed) {
				dis->init(128, 64, 4);
				n_emu_state = STATE_INIT;
			}

			/********
			 * Oh boy...
			 * 
			 * If ya couldn't tell, this takes the bitmapped array
			 * `n_display_buffer_data`, and loops over its bits,
			 * it then scales these 'pixels' by the scalar value onto the canvas.
			 */
			for (int n_buffer_idx = 0; n_buffer_idx < dis->n_display_buffer_len; n_buffer_idx++) {
				for (int n_position = 0; n_position < 8; n_position++) {
					// Ensures most significant bit is leftmost.
					uint8_t mask = 1 << 7 - n_position;
					for (int n_offset_x = 0; n_offset_x < dis->n_display_scale; n_offset_x++) {
						for (int n_offset_y = 0; n_offset_y < dis->n_display_scale; n_offset_y++) {
							Draw(
								(n_buffer_idx * 8 + n_position) % 
									dis->n_display_width * dis->n_display_scale + n_offset_x + CANVAS_OFFSET,
								(n_buffer_idx * 8 + n_position) /
									dis->n_display_width * dis->n_display_scale + n_offset_y + CANVAS_OFFSET,
								(dis->n_display_buffer_data[n_buffer_idx] & mask) ? olc::GREEN : olc::BLACK
							);
						} // n_offset_y
					} // n_offset_x
				} // n_position
			} // n_buffer_idx

			for (uint16_t pc = n_hexview_base_addr; pc < n_hexview_base_addr + 0x1d0; pc += 8) {
				char str[32];
				sprintf(
					str,
					"%04X: %02X %02X %02X %02X %02X %02X %02X %02X",
					pc,
					chip->n_memory[pc + 0],
					chip->n_memory[pc + 1],
					chip->n_memory[pc + 2],
					chip->n_memory[pc + 3],
					chip->n_memory[pc + 4],
					chip->n_memory[pc + 5],
					chip->n_memory[pc + 6],
					chip->n_memory[pc + 7]
				);

				DrawString(
					dis->n_display_width * dis->n_display_scale + CANVAS_OFFSET * 2,
					((pc - n_hexview_base_addr) + CANVAS_OFFSET),
					str
				);
			}

			for (uint16_t pc = chip->n_program_counter; pc < chip->n_program_counter + 50; pc += 2) {
				char str[32];
				chip->disassemble((char*) &str, pc);

				DrawString(
					CANVAS_OFFSET,
					dis->n_display_height * dis->n_display_scale + CANVAS_OFFSET * ((pc - chip->n_program_counter) / 2 + 2),
					(pc == chip->n_program_counter) ? "*" : " ",
					(pc == chip->n_program_counter) ? olc::CYAN : olc::WHITE
				);

				DrawString(
					CANVAS_OFFSET * 2,
					dis->n_display_height * dis->n_display_scale + CANVAS_OFFSET * ((pc - chip->n_program_counter) / 2 + 2),
					str,
					(pc == chip->n_program_counter) ? olc::CYAN : olc::WHITE
				);
			}

			for (uint16_t i = 0; i < 16; i++) {
				char str[12];
				sprintf(str, "V[%X]: %02X", i, chip->n_register[i]);

				DrawString(
					CANVAS_OFFSET * 18,
					dis->n_display_height * dis->n_display_scale + CANVAS_OFFSET * (i + 2),
					str
				);
			}

			return true;
		}
	};
}

int main(int argc, char** argv) {
	Chippy::Chippy emulator;
	if (emulator.Construct(768, 480, 1, 1, false))
		emulator.Start();

	return 0;
}