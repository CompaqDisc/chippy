#define OLC_PGE_APPLICATION
#include <stdint.h>
#include "olcPixelGameEngine.h"
#include "display.h"
#include "chip8.h"

#define CANVAS_OFFSET	8

namespace Chippy
{
	class Chippy : public olc::PixelGameEngine
	{
	public:
		enum EmulatorState
		{
			STATE_INIT,
			STATE_MENU,
			STATE_RUNNING,
			STATE_EXITING
		};

		Display*	display;
		Chip8*		chip;
		uint16_t	n_hexview_base_addr;
		uint32_t	n_emu_state;
		const char*		s_test_fname;
		
		Chippy()
		{
			this->sAppName			= "Chippy";
			this->n_emu_state		= STATE_INIT;
			this->n_hexview_base_addr	= 0x200;
			this->display			= new Display();
			this->chip			= new Chip8();
			this->s_test_fname		= "Pong [Paul Vervalin, 1990].ch8";
		}

		bool OnUserCreate() override
		{
			display->init(Display::MODE_CHIP8);
			chip->setmode(Chip8::MODE_CHIP8);
			chip->reset();
			if (!chip->loadmem(this->s_test_fname))
				return false;
			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override
		{
			Clear(olc::VERY_DARK_GREY);

			if (GetKey(olc::Key::ESCAPE).bPressed)
			{
				delete display;
				delete chip;
				return false;
			}

			if (GetKey(olc::Key::PGDN).bHeld)
			{
				if (!(n_hexview_base_addr >= (0xfff - 0x1d0)))
					n_hexview_base_addr += 8;
			}

			if (GetKey(olc::Key::PGUP).bHeld)
			{
				if (!(n_hexview_base_addr <= 0x000))
					n_hexview_base_addr -= 8;
			}

			if (GetKey(olc::Key::HOME).bPressed)
			{
				display->init(Display::MODE_CHIP8);
				chip->setmode(Chip8::MODE_CHIP8);
				chip->reset();
				n_emu_state = STATE_INIT;
			}

			if (GetKey(olc::Key::END).bPressed)
			{
				display->init(Display::MODE_CHIP48);
				chip->setmode(Chip8::MODE_CHIP48);
				chip->reset();
				n_emu_state = STATE_INIT;
			}

			if (GetKey(olc::Key::SPACE).bPressed)
			{
				if(!chip->step())
					return false;
			}

			// If we are in vanilla chip8 mode copy display memory to the display.
			if (chip->n_current_mode == Chip8::MODE_CHIP8)
			{
				for (uint16_t i = 0xf00; i < 0x1000; i++)
				{
					display->n_display_buffer_data[i - 0xf00] = chip->n_memory[i];
				}
			}

			/********
			 * Oh boy...
			 * 
			 * If ya couldn't tell, this takes the bitmapped array
			 * `n_display_buffer_data`, and loops over its bits,
			 * it then scales these 'pixels' by the scalar value onto the canvas.
			 */
			for (int n_buffer_idx = 0; n_buffer_idx < display->n_display_buffer_len; n_buffer_idx++)
			{
				for (int n_position = 0; n_position < 8; n_position++)
				{
					// Ensures most significant bit is leftmost.
					uint8_t mask = 1 << 7 - n_position;
					for (int n_offset_x = 0; n_offset_x < display->n_display_scale; n_offset_x++)
					{
						for (int n_offset_y = 0; n_offset_y < display->n_display_scale; n_offset_y++)
						{
							Draw(
								(n_buffer_idx * 8 + n_position) % 
									display->n_display_width * display->n_display_scale + n_offset_x + CANVAS_OFFSET,
								(n_buffer_idx * 8 + n_position) /
									display->n_display_width * display->n_display_scale + n_offset_y + CANVAS_OFFSET,
								(display->n_display_buffer_data[n_buffer_idx] & mask) ? olc::GREEN : olc::BLACK
							);
						} // n_offset_y
					} // n_offset_x
				} // n_position
			} // n_buffer_idx

			// Draw hexview.
			for (uint16_t pc = n_hexview_base_addr; pc < n_hexview_base_addr + 0x1d0; pc += 8)
			{
				char str[32];
				sprintf(
					str,
					"%03X: %02X %02X %02X %02X %02X %02X %02X %02X",
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
					display->n_display_width * display->n_display_scale + CANVAS_OFFSET * 2,
					((pc - n_hexview_base_addr) + CANVAS_OFFSET),
					str
				);
			}

			// Draw scrollbar background.
			FillRect(
				display->n_display_width * display->n_display_scale + CANVAS_OFFSET * 30.5,
				CANVAS_OFFSET,
				CANVAS_OFFSET / 2,
				CANVAS_OFFSET * 58,
				olc::BLACK
			);

			// Draw scrollbar innards.
			FillRect(
				display->n_display_width * display->n_display_scale + CANVAS_OFFSET * 30.5,
				CANVAS_OFFSET + n_hexview_base_addr / 8,
				CANVAS_OFFSET / 2,
				CANVAS_OFFSET * 1.25,
				olc::WHITE
			);

			// Draw dissasembly.
			for (uint16_t pc = chip->n_program_counter; pc < chip->n_program_counter + 50; pc += 2)
			{
				char str[32];
				chip->disassemble(str, pc);

				DrawString(
					CANVAS_OFFSET,
					display->n_display_height * display->n_display_scale + CANVAS_OFFSET * ((pc - chip->n_program_counter) / 2 + 2),
					(pc == chip->n_program_counter) ? "*" : " ",
					(pc == chip->n_program_counter) ? olc::CYAN : olc::WHITE
				);

				DrawString(
					CANVAS_OFFSET * 2,
					display->n_display_height * display->n_display_scale + CANVAS_OFFSET * ((pc - chip->n_program_counter) / 2 + 2),
					str,
					(pc == chip->n_program_counter) ? olc::CYAN : olc::WHITE
				);
			}

			// Draw program counter.
			{
				char str[8];
				sprintf(str, "PC: %03x", chip->n_program_counter);

				DrawString(
					CANVAS_OFFSET * 23,
					display->n_display_height * display->n_display_scale + CANVAS_OFFSET * 2,
					str
				);
			}

			// Draw I register contents.
			{
				char str[12];
				sprintf(str, "I : %03x", chip->n_i_register);

				DrawString(
					CANVAS_OFFSET * 23,
					display->n_display_height * display->n_display_scale + CANVAS_OFFSET * 3,
					str
				);
			}

			// Draw stack pointer.
			{
				char str[8];
				sprintf(str, "SP: %02x", chip->n_stack_pointer);

				DrawString(
					CANVAS_OFFSET * 23,
					display->n_display_height * display->n_display_scale + CANVAS_OFFSET * 4,
					str
				);
			}

			// Draw generic register contents.
			for (uint16_t i = 0; i < 16; i++) {
				char str[12];
				sprintf(str, "V[%X]: %02X", i, chip->n_register[i]);

				DrawString(
					CANVAS_OFFSET * 23,
					display->n_display_height * display->n_display_scale + CANVAS_OFFSET * (i + 11),
					str
				);
			}

			return true;
		}
	};
}

int main(int argc, char** argv)
{
	Chippy::Chippy* emulator = new Chippy::Chippy();
	if (emulator->Construct(768, 480, 1, 1, false))
		emulator->Start();

	return 0;
}