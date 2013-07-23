PAssembly
========

## Instructions

PAssembly commands consist instruction and zero to one argument. Argument is always value between 0 - 0xFF.
It can be constant value or address depending on instruction. Some insturctions can also have :R or :W 
versions. :R means that result of the insturuction is writen back to memory and :W means that it is
writen to wreg.


# LDC - Load Data to Constant
arg -> wreg

Loads constant value of argument to werg.

LDC 20 // Sets wreg to 20

# STR - Store To Ram
wreg -> ram[arg]

Stores current data from wreg to memory.

LDC 20		// wreg = 20
STR 0xFF	// 20 is stored to address 0xFF

# LDR - Load Data from Ram
ram[arg] -> wreg

Loads data from ram back to wreg.

LDC 20		// wreg = 20
STR 0xFF	// 20 is stored to address 0xFF
LDC 40		// wreg = 40
LDR 255		// wreg = 20 again

 
# ADD - Addition
:R  wreg + ram[arg] -> ram[arg]
:W  wreg + ram[arg] -> wreg

Adds wreg to data in ram and stores result to wreg or back to same memory loaction.
If result overflow, carry bit is set to 1. Else it is set to 0.

LDC 30		// wreg = 30
STR 0xFF	// wreg = 30
LDC 1		// wreg = 1
ADD:W 0xFF	// wreg = 31

# SUB - Subtraction
:R  wreg - ram[arg] -> ram[arg]
:W  wreg - ram[arg] -> wreg

Subtracts data in ram from wreg and stores result to wreg or back to same memory loaction.
If result overflow, carry bit is set to 1. Else it is set to 0.

LDC 30		// wreg = 30
STR 0xFF	// wreg = 30
LDC 50		// wreg = 50 
ADD:R 0xFF	// wreg = 50, ram[0xFF] = 20

# AND - binary wise and operation
:R  wreg & ram[arg] -> ram[arg]
:W  wreg & ram[arg] -> wreg

LDC 0b00001111		// wreg = 15
STR 0xFF	
LDC 0b11000011		// wreg = 0b11000011
AND:W 0xFF			// wreg = 0b00000011, ram[0xFF] = 0b11000011

# OR - binary wise and operation
:R  wreg | ram[arg] -> ram[arg]
:W  wreg | ram[arg] -> wreg

LDC 0b00001111		// wreg = 15
STR 0xFF	
LDC 0b11000011		// wreg = 0b11000011
OR:W 0xFF			// wreg = 0b11001111, ram[0xFF] = 0b11000011

# XOR - binary wise and operation
:R  wreg ^ ram[arg] -> ram[arg]
:W  wreg ^ ram[arg] -> wreg

LDC 0b00001111		// wreg = 15
STR 0xFF	
LDC 0b11000011		// wreg = 0b11000011
XOR:R 0xFF			// wreg = 0b11000011, ram[0xFF] = 0b11001100

# INC - Increase
ram[arg] + 1 -> ram[arg]

Value in memory location is increased by 1.
If result overflow, carry bit is set to 1. Else it is set to 0.

LDC 20	
STR 0xF4		// ram[0xF4] = 20
INC 0xF4		// ram[0xF4] = 21

# DEC - Decrease
ram[arg] - 1 -> ram[arg]

Value in memory location is decreased by 1.
If result overflow, carry bit is set to 1. Else it is set to 0.

LDC 20	
STR 0xF4		// ram[0xF4] = 20
DEC 0xF4		// ram[0xF4] = 19

# SR - Shift Right
ram[arg] >> 1 -> ram[arg]

LDC 0b01001000 
STR 0xF0		// ram[0xF0] = 0b01001000
SR 0xF0			// ram[0xF0] = 0b00100100


# SR - Shift Right
ram[arg] << 1 -> ram[arg]

LDC 0b01001000 
STR 0xF0		// ram[0xF0] = 0b01001000
SR 0xF0			// ram[0xF0] = 0b10010000

# UDW - Update with wreg
Exits assembly code and returns to main code. This must be done frequently enough
to give time to main program to read serial and update leds. This also makes
assembly program to sleep 10 ms * wreg.

LDC 5
UDW 	// Program updates leds and sleeps 50 ms.

# UDC - Update with constant
Exits assembly code and returns to main code. This must be done frequently enough
to give time to main program to read serial and update leds. This also makes
assembly program to sleep 10 ms * argument.

UDW 10	// Program updates leds and sleeps 100 ms.


# SIRZ - Skip If Register Zero
Skip next instruction if ram[arg] is zero.

LDC 0
STR 0xFF
SIRZ 0xFF
LDC 10			// This is skpipped

# JIWZ - Jump If Wreg Zero
Jumps to address of argument if wreg is zero

LDC 0
JIWZ 0x03
LDC 2
LDC 4		// Program jumps here
LDC 5

# JICZ - Jump If Carry Zero
Jump to address of argument if carry is zero


# JMP - Jump
arg -> PC
Jumps program execution to position of argument.

JMP 0x02
LDC 23
LDC 12		// Jumps here

# RJMP - Relative jump
PC + arg -> PC

# RST - Reset
Reset machine state (Wreg, PC,carry and ram).

#SIWE - Skip If Wreg Equals

#SIWB - Skip If Wreg is Bigger

#SIWS - Skip If Wreg is Smaller

#SIWZ - Skpi If Wreg is Zero

#SICZ - Skpi If Carry Zero

#RET - Return 
ram[0x2D] -> PC
Jumps to position stored in memory loaction 0x2D. This can be used
for making basic functions.







## Memory

0x00 Vector 1: Red
0x01 Vector 1: Green
0x02 Vector 1: Blue
0x03 Vector 1: Direction
0x04 Vector 1: Sector
0x05 Vector 2: Red
0x06 Vector 2: Green
0x07 Vector 2: Blue
0x08 Vector 2: Direction
0x09 Vector 2: Sector
0x0A Vector 3: Red
0x0B Vector 3: Green
0x0C Vector 3: Blue
0x0D Vector 3: Direction
0x0E Vector 3: Sector
0x0F Vector 4: Red
0x10 Vector 4: Green
0x11 Vector 4: Blue
0x12 Vector 4: Direction
0x13 Vector 4: Sector
0x14 Vector 5: Red
0x15 Vector 5: Green
0x16 Vector 5: Blue
0x17 Vector 5: Direction
0x18 Vector 5: Sector
0x19 Vector 6: Red
0x1A Vector 6: Green
0x1B Vector 6: Blue
0x1C Vector 6: Direction
0x1D Vector 6: Sector
0x1E Vector 7: Red
0x1F Vector 7: Green
0x20 Vector 7: Blue
0x21 Vector 7: Direction
0x22 Vector 7: Sector
0x23 Vector 8: Red
0x24 Vector 8: Green
0x25 Vector 8: Blue
0x26 Vector 8: Direction
0x27 Vector 8: Sector

0x28 Pointer address
0x29 Pointer's data

0x2A Read -> Random value, Write -> Set seed

0x2B Read -> Battery level (mV), upper bytes
0x2C Read -> Battery level (mV), lower bytes

0x2D Return address for RET instruction

0x2C - 0x3F RESERVED

0x40 - 0xFF General purpose memory



