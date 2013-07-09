
#include "animation.h"
#include "battery.h"
#include <stdlib.h>


uint8_t gp_ram[64];
uint8_t indirectAddress = 0;
uint8_t PC;
uint8_t returnAddress = 0;

void ramStore (const uint8_t addr, const uint8_t data)
{
	if (addr < 40)
	{
		uint8_t* ptr = (uint8_t*)vectors;
		*(ptr + addr) = data;
	}
	else if (addr == 40)
	{
		indirectAddress = data;
	}
	else if (addr == 41)
	{
		//Let's make sure that we dont go to infinite recursion
		if (indirectAddress != 41)
			ramStore(indirectAddress, data);
	}
	else if (addr == 42)
	{
		srand(data);
	}
	else if (addr == 45)
	{
		returnAddress = data;
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
	else if (addr == 40)
	{
		return indirectAddress;
	}
	else if (addr == 41)
	{
		//Let's make sure that we dont go to infinite recursion
		if (indirectAddress != 41)
			return ramLoad(indirectAddress);
	}
	else if (addr == 42)
	{
		return rand();
	}
	else if (addr == 43)
	{
		return (uint8_t)(batteryManager::getBatteryLevel() >> 8);
	}
	else if (addr == 44)
	{
		return (uint8_t)batteryManager::getBatteryLevel();
	}
	else if (addr == 45)
	{
		return returnAddress;
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

void ramReset(){
	uint8_t* ptr = (uint8_t*)vectors;
	for (uint8_t i = 0; i < 40; i++)
		*(ptr + i) = 0;
	indirectAddress = 0;

}

void resetPC()
{
	PC = 0;
}


uint8_t runAnimationCode(uint16_t* code)
{

	uint8_t wdc = 64;
	static uint8_t wreg = 0;	
	static uint8_t carry = 0;

	while (wdc--){

		uint8_t opcode = (uint8_t)(code[PC] >> 8);
		uint8_t data  = (uint8_t)code[PC];
		uint16_t tmp;
		uint16_t tmp8;
	
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
			tmp = (uint16_t)ramLoad(data) + (uint16_t)wreg;
			carry = (uint8_t)(tmp >> 8);

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_SUB:
			tmp = (uint16_t)wreg - (uint16_t)ramLoad(data);
			carry = (uint8_t)(tmp >> 8);

			if (opcode & STORE_WREG_MASK)  wreg = (uint8_t)tmp;
			else ramStore(data, (uint8_t)tmp);
			break;

		case OPCODE_AND:
			tmp8 = ramLoad(data) & wreg;
			if (opcode & STORE_WREG_MASK)  wreg = tmp8;
			else ramStore(data, tmp8);
			break;

		case OPCODE_OR:
			tmp8 = ramLoad(data) | wreg;
			if (opcode & STORE_WREG_MASK)  wreg = tmp8;
			else ramStore(data, tmp8);
			break;

		case OPCODE_XOR:
			tmp8 = ramLoad(data) ^ wreg;
			if (opcode & STORE_WREG_MASK)  wreg = tmp8;
			else ramStore(data, tmp8);
			break;

		case OPCODE_INC:
			tmp8 = ramLoad(data);
			if (tmp8 == 0xFF) carry = 1;
			else carry = 0;
			tmp8++;
			ramStore(data, tmp8);
			break;

		case OPCODE_DEC:
			tmp8 = ramLoad(data);
			if (tmp8 == 0x00) carry = 1;
			else carry = 0;			
			tmp8--;
			ramStore(data, tmp8);
			break;

		case OPCODE_SHIFT_R:
			carry = wreg & 0x01;
			wreg >>= 1;
			if(data) wreg |= 0x80;			
			break;

		case OPCODE_SHIFT_L:
			carry = (wreg >> 7) & 0x01;
			wreg <<= 1;
			if(data) wreg |= 0x01;
			break;

		case OPCODE_UPDATE_WREG:
			return wreg;

		case OPCODE_UPDATE_CONST:
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

		case OPCODE_JIWZ:
			if (wreg == 0) PC = data;
			break;

		case OPCODE_JICZ:
			if (carry == 0) PC = data;
			break;

		case OPCODE_JUMP:
			PC = data;
			break;
		case OPCODE_REL_JUMP:
			PC += data;
			break;

		case OPCODE_RST:
			PC = data;
			wreg = 0;
			carry = 0;
			ramReset();
			return 0;	

		case OPCODE_SIWE:
			if(ramLoad(data) == wreg)
				PC++;
			break;

		case OPCODE_SIWB:
			if(ramLoad(data) < wreg)
				PC++;
			break;

		case OPCODE_SIWS:
			if(ramLoad(data) > wreg)
				PC++;
			break;

		case OPCODE_RET:
			PC = returnAddress;
			break;

		default:
			break;

		}
	}

	return 0;
}




