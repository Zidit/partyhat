
#include "animation.h"
#include <stdlib.h>


uint8_t gp_ram[64];

void ramStore (const uint8_t addr, const uint8_t data)
{
	if (addr < 40)
	{
		uint8_t* ptr = (uint8_t*)vectors;
		*(ptr + addr) = data;
	}
	else if (addr < 41)
	{
		return srand(data);
	}
	else if (addr < 64)
	{
		//Reserved to later use
	} 
	else if (addr < 128)
	{
		//general purpose memory
		gp_ram[addr - 64] = data;
	}

}

uint8_t ramLoad (const uint8_t addr)
{
	if (addr < 40)
	{
		uint8_t* ptr = (uint8_t*)vectors;
		return *(ptr + addr);
	}
	else if (addr < 41)
	{
		return rand();
	}
	else if (addr < 64)
	{
		//Reserved to later use
		return 0;
	} 
	else if (addr < 128)
	{
		//general purpose memory
		return gp_ram[addr - 64];
	}

	return 0;
}

uint8_t runAnimationCode(uint16_t* code)
{

	uint8_t wdc = 64;
	static uint8_t PC = 0;	
	static uint8_t wreg = 0;	
	static uint8_t carry = 0;

	while (wdc--){

		uint8_t opcode = (uint8_t)(code[PC] >> 8);
		uint8_t data  = (uint8_t)code[PC];
		uint16_t tmp;
	
		PC++;

		switch (opcode & OPCODE_MASK)
		{
		case OPCODE_LOAD_CONST:
			wreg = data;
			break;

		case OPCODE_LOAD_RAM:
			wreg = ramLoad(data);
			break;

		case OPCODE_STORE_RAM:
			ramStore(data, wreg);
			break;

		case OPCODE_ADD:
			tmp = ramLoad(data) + wreg;
			carry = (uint8_t)(tmp >> 8) & 0x01;

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_SUB:
			tmp = wreg - ramLoad(data);
			carry = (uint8_t)(tmp >> 8) & 0x01;

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_AND:
			tmp = ramLoad(data) & wreg;

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCEOE_OR:
			tmp = ramLoad(data) | wreg;

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_XOR:
			tmp = ramLoad(data) ^ wreg;

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_INC:
			tmp = ramLoad(data) + 1;
			carry = (uint8_t)(tmp >> 8) & 0x01;
			ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_DEC:
			tmp = ramLoad(data) - 1;
			carry = (uint8_t)(tmp >> 8) & 0x01;
			ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_SHIFT_R:
			carry = wreg & 0x01;
			wreg >>= 1;			
			break;

		case OPCODE_SHIFT_L:
			carry = (wreg >> 7) & 0x01;
			wreg <<= 1;
			break;

		case OPCODE_RET_M_WREG:
			return wreg;

		case OPCODE_RET_M_CONST:
			return data;

		case OPCODE_SIWZ:
			if (wreg == 0) PC++;
			break;

		case OPCODE_SIRZ:
			if (ramLoad(data) == 0) PC++;
			break;

		case OPCODE_SICZ:
			if (carry == 0) PC++;
			break;

		case OPCODE_JUMP:
			PC = data;
			break;

		}
	}

	return 0;
}




