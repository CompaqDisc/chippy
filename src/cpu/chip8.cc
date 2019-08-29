#include <stdint.h>
#include <cstdio>
#include "chip8.h"

#define V0 0x0
#define V1 0x1
#define V2 0x2
#define V3 0x3
#define V4 0x4
#define V5 0x5
#define V6 0x6
#define V7 0x7
#define V8 0x8
#define V9 0x9
#define VA 0xA
#define VB 0xB
#define VC 0xC
#define VD 0xD
#define VE 0xE
#define VF 0xF

Chippy::Chip8::Chip8()
{
	this->reset();
	// for (uint16_t i = 0x000; i < 0x1000; i++)
	// 	this->n_memory[i] = i ^ (i >> 8) & 0xff;
	for (uint16_t i = 0x000; i < 0x1000; i++)
		this->n_memory[i] = 0x00;
}

void Chippy::Chip8::reset()
{
	this->n_stack_pointer	= 0x00;
	this->n_program_counter	= 0x200;
	this->n_i_register	= 0x000;
	for (uint8_t i = 0; i < 16; i++)
		this->n_register[i] = 0x00;
}

void Chippy::Chip8::setmode(uint32_t mode)
{
	this->n_current_mode = mode;
}

bool Chippy::Chip8::loadmem(const char* filename)
{
	FILE* f = nullptr;
	f = fopen(filename, "rb");

	if (!f)
	{
		fprintf(stderr, "Error opening %s.\n", filename);
		return false;
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

	fread(&this->n_memory[0x200], size, 1, f);

	fclose(f);
	return true;
}

bool Chippy::Chip8::step()
{
	switch (this->n_memory[this->n_program_counter] >> 4)
	{
	case 0x0:
		if (this->n_memory[this->n_program_counter + 1] == 0xe0)
		{
			// CLS
			// Clear the display.
			for (uint16_t i = 0xf00; i < 0x1000; i++)
				this->n_memory[i] = 0x00;
		}
		else if (this->n_memory[this->n_program_counter + 1] == 0xee)
		{
			// RET
			// Return from a subroutine.
			// The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
			this->n_program_counter =
				(uint16_t) (this->n_memory[0xEA0 + this->n_stack_pointer * 2] << 8) | this->n_memory[(0xEA0 + this->n_stack_pointer * 2) + 1];

			this->n_stack_pointer--;
			this->n_program_counter -= 2;
		}
		else
		{
			// SYS nnn
			// Do nothing.
		}
		break;
	case 0x2:
		// CALL
		// The interpreter increments the stack pointer,
		this->n_stack_pointer++;

		// then puts the current PC on the top of the stack.
		this->n_memory[0xEA0 + (this->n_stack_pointer * 2)] = (uint8_t) (this->n_program_counter >> 8);
		this->n_memory[0xEA0 + ((this->n_stack_pointer * 2) + 1)] = (uint8_t) (this->n_program_counter & 0xFF);

		// The PC is then set to nnn.
		this->n_program_counter = (uint16_t) ((this->n_memory[this->n_program_counter] & 0xf) << 8) | this->n_memory[this->n_program_counter + 1];
		this->n_program_counter -= 2;

		break;
	case 0x6:
		// LD Vx, byte
		this->n_register[this->n_memory[this->n_program_counter] & 0xf] = this->n_memory[this->n_program_counter + 1];
		break;
	case 0xa:
		// LD I, addr
		this->n_i_register = (uint16_t) ((this->n_memory[this->n_program_counter] & 0xf) << 8) | this->n_memory[this->n_program_counter + 1];
		break;
	case 0xd:
		{
			// DRW Vx, Vy, nibble
			uint8_t sprite_len 	= this->n_memory[this->n_program_counter + 1] & 0xf;
			bool 	collision	= 0;

			uint16_t n_sprite_base  = this->n_i_register;
			uint16_t n_display_base = 0xF00;

			// Loop over sprite data.
			for (int8_t i = 0; i < sprite_len; i++)
			{
				// Paint sprite bits in left-to-right order (msb first).
				for(int8_t b = 7; b >= 0; b--)
				{
					// Get a linear pixel number from x and y.
					uint8_t x = this->n_register[this->n_memory[this->n_program_counter] & 0xf] + (7 - b);
					uint8_t y = this->n_register[(this->n_memory[this->n_program_counter + 1] >> 4) & 0xf] + i;
					uint16_t n_pixel = y * 64 + x;

					// Get an offset into display memory.
					uint16_t n_pixel_offset = n_pixel / 8;
					// Get the display bit.
					uint8_t n_display_bit_n = 7 - (n_pixel % 8);

					#ifdef DEBUG
					printf("[%d](%02d,%02d): 0x%03X&(1<<%d)\n", b, x, y, n_display_base + n_pixel_offset, n_display_bit_n);
					#endif

					uint8_t n_sprite_data  = this->n_memory[n_sprite_base  + i];
					uint8_t n_display_data = this->n_memory[n_display_base + n_pixel_offset];

					// Get bit from each.
					uint8_t n_sprite_bit  = (n_sprite_data & (1 << b)) >> b;
					uint8_t n_display_bit = (n_display_data & (1 << n_display_bit_n)) >> n_display_bit_n;

					// Test collision by logic and.
					if (n_sprite_bit & n_display_bit) collision = 1;

					// Do the XOR onto screen memory.
					n_display_data ^= (n_sprite_bit << n_display_bit_n);
					this->n_memory[n_display_base + n_pixel_offset] = n_display_data;
				}
			}

			this->n_register[VF] = collision;
		}
		break;
	case 0xf:
		switch (this->n_memory[this->n_program_counter + 1])
		{
		case 0x33:
			// LD B, Vx
			{
				uint8_t vx = this->n_register[this->n_memory[this->n_program_counter] & 0xf];
				uint8_t tmp = vx;
				uint8_t hundreds = tmp / 100;
				tmp %= 100;
				uint8_t tens = tmp / 10;
				tmp %= 10;
				uint8_t ones = tmp;

				#ifdef DEBUG
				printf("(%d) %d-%d-%d\n", vx, hundreds, tens, ones);
				#endif

				this->n_memory[this->n_i_register    ] = hundreds;
				this->n_memory[this->n_i_register + 1] = tens;
				this->n_memory[this->n_i_register + 2] = ones;
			}
			break;
		case 0x65:
			// LD Vx, [I]
			// The interpreter reads values from memory starting at location I into registers V0 through Vx.
			{
				//
			}
			//break;
		default:
			char str[32];
			this->disassemble(str, this->n_program_counter);

			fprintf(stderr, "Error executing: %s\n", str);
			return false;
		}
		break;
	default:
		char str[32];
		this->disassemble(str, this->n_program_counter);

		fprintf(stderr, "Error executing: %s\n", str);
		return false;
	}

	this->n_program_counter += 2;
	return true;
}

void Chippy::Chip8::disassemble(char* str, uint16_t addr)
{
	switch (this->n_memory[addr] >> 4)
	{
	case 0x0:
		if (this->n_memory[addr + 1] == 0xe0)
			sprintf(str, "%03X: CLS", addr);
		else if (this->n_memory[addr + 1] == 0xee)
			sprintf(str, "%03X: RET", addr);
		else
		{
			sprintf(
				str, "%03X: SYS %03X", addr,
				(uint16_t) ((this->n_memory[addr] & 0xf) << 8 | this->n_memory[addr + 1])
			);
		}
		break;

	case 0x1:
		sprintf(
			str, "%03X: JP %03X", addr,
			(uint16_t) ((this->n_memory[addr] & 0xf) << 8 | this->n_memory[addr + 1])
		);
		break;

	case 0x2:
		sprintf(
			str, "%03X: CALL %03X", addr,
			(uint16_t) ((this->n_memory[addr] & 0xf) << 8 | this->n_memory[addr + 1])
		);
		break;

	case 0x3:
		sprintf(
			str, "%03X: SE V%X %02X", addr,
			(this->n_memory[addr] & 0xf),
			this->n_memory[addr + 1]
		);
		break;

	case 0x4:
		sprintf(
			str, "%03X: SNE V%X %02X", addr,
			(this->n_memory[addr] & 0xf),
			this->n_memory[addr + 1]
		);
		break;

	case 0x5:
		if ((this->n_memory[addr + 1] & 0xf) == 0)
		{
			sprintf(
				str, "%03X: SE V%X V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
		}
		else
		{
			sprintf(
				str, "%03X: ILLEGAL (%02X%02X)", addr,
				this->n_memory[addr],
				this->n_memory[addr + 1]
			);
		}
		break;

	case 0x6:
		sprintf(
			str, "%03X: LD V%X, %02X", addr,
			(this->n_memory[addr] & 0xf),
			this->n_memory[addr + 1]
		);
		break;

	case 0x7:
		sprintf(
			str, "%03X: ADD V%X, %02X", addr,
			(this->n_memory[addr] & 0xf),
			this->n_memory[addr + 1]
		);
		break;

	case 0x8:
		switch (this->n_memory[addr + 1] & 0xf)
		{
		case 0x0:
			sprintf(
				str, "%03X: LD V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x1:
			sprintf(
				str, "%03X: OR V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x2:
			sprintf(
				str, "%03X: AND V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x3:
			sprintf(
				str, "%03X: XOR V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x4:
			sprintf(
				str, "%03X: ADD V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x5:
			sprintf(
				str, "%03X: SUB V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x6:
			sprintf(
				str, "%03X: SHR V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0x7:
			sprintf(
				str, "%03X: SUBN V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		case 0xe:
			sprintf(
				str, "%03X: SHL V%X, V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
			break;
		default:
			sprintf(
				str, "%03X: ILLEGAL (%02X%02X)", addr,
				this->n_memory[addr],
				this->n_memory[addr + 1]
			);
			break;
		}
		break;
	case 0x9:
		if ((this->n_memory[addr + 1] & 0xf) == 0)
		{
			sprintf(
				str, "%03X: SNE V%X V%X", addr,
				(this->n_memory[addr] & 0xf),
				(this->n_memory[addr + 1] >> 4)
			);
		} else
		{
			sprintf(
				str, "%03X: ILLEGAL (%02X%02X)", addr,
				this->n_memory[addr],
				this->n_memory[addr + 1]
			);
		}
		break;
	case 0xa:
		sprintf(
			str, "%03X: LD I, %03X", addr,
			(uint16_t) ((this->n_memory[addr] & 0xf) << 8 | this->n_memory[addr + 1])
		);
		break;
	case 0xb:
		sprintf(
			str, "%03X: JP V0, %03X", addr,
			(uint16_t) ((this->n_memory[addr] & 0xf) << 8 | this->n_memory[addr + 1])
		);
		break;
	case 0xc:
		sprintf(
			str, "%03X: RND V%X, %02X", addr,
			(this->n_memory[addr] & 0xf),
			this->n_memory[addr + 1]
		);
		break;
	case 0xd:
		sprintf(
			str, "%03X: DRW V%X, V%X, %X", addr,
			(this->n_memory[addr] & 0xf),
			(this->n_memory[addr + 1] >> 4),
			(this->n_memory[addr + 1] & 0xf)
		);
		break;
	case 0xe:
		if (this->n_memory[addr + 1] == 0x9e)
		{
			sprintf(
				str, "%03X: SKP V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
		}
		else if (this->n_memory[addr + 1] == 0xa1)
		{
			sprintf(
				str, "%03X: SKNP V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
		}
		else
		{
			sprintf(
				str, "%03X: ILLEGAL (%02X%02X)", addr,
				this->n_memory[addr],
				this->n_memory[addr + 1]
			);
		}
		break;
	case 0xf:
		switch (this->n_memory[addr + 1])
		{
		case 0x07:
			sprintf(
				str, "%03X: LD V%X, DT", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x0a:
			sprintf(
				str, "%03X: LD V%X, K", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x15:
			sprintf(
				str, "%03X: LD DT, V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x18:
			sprintf(
				str, "%03X: LD ST, V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x1e:
			sprintf(
				str, "%03X: ADD I, V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x29:
			sprintf(
				str, "%03X: LD F, V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x33:
			sprintf(
				str, "%03X: LD B, V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x55:
			sprintf(
				str, "%03X: LD [I], V%X", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		case 0x65:
			sprintf(
				str, "%03X: LD V%X, [I]", addr,
				(this->n_memory[addr] & 0xf)
			);
			break;
		default:
			sprintf(
				str, "%03X: ILLEGAL (%02X%02X)", addr,
				this->n_memory[addr],
				this->n_memory[addr + 1]
			);
			break;
		}
		break;
	default:
		sprintf(
			str, "%03X: ILLEGAL (%02X%02X)", addr,
			this->n_memory[addr],
			this->n_memory[addr + 1]
		);
		break;
	}
}
