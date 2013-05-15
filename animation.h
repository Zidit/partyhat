#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

#include "config.h"
#include "vectors.h"

#define OPCODE_MASK 		0b01111111
#define STORE_WREG_MASK 	0b10000000

#define OPCODE_LOAD_CONST	0x01
#define OPCODE_LOAD_RAM 	0x02
#define OPCODE_STORE_RAM 	0x03
#define OPCODE_ADD			0x04
#define OPCODE_SUB			0x05
#define OPCODE_AND			0x06
#define OPCEOE_OR			0x07
#define OPCODE_XOR			0x08
#define OPCODE_INC			0x09
#define OPCODE_DEC			0x0A
#define OPCODE_SHIFT_R		0x0B
#define OPCODE_SHIFT_L		0x0C
#define OPCODE_RET_M_WREG 	0x10
#define OPCODE_RET_M_CONST	0x11
#define OPCODE_SIWZ			0x12
#define OPCODE_SIRZ			0x13
#define OPCODE_SICZ			0x14
#define OPCODE_JUMP			0x15


uint8_t runAnimationCode(uint16_t* code);


#endif // ANIMATION_H_INCLUDED
