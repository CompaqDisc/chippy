#include <stdint.h>
#include <cstdio>
#include "chip8.h"

Chippy::Chip8::Chip8()
{
	this->reset();
}

void Chippy::Chip8::reset()
{
	this->n_stack_pointer	= 0x00;
	this->n_program_counter	= 0x200;
	this->n_i_register	= 0x000;
	for (uint16_t i = 0x000; i < 0x1000; i++)
		this->n_memory[i] = i ^ (i >> 8) & 0xff;
	this->n_memory[0x200] = 0xd5;
}

void Chippy::Chip8::setmode(uint32_t mode)
{
	this->n_current_mode = mode;
}

void Chippy::Chip8::step()
{
	this->n_program_counter += 2;
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