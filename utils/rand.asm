#define vec0.r 0x00
#define vec0.g 0x01
#define vec0.b 0x02
#define vec0.d 0x03
#define vec0.s 0x04

#define vec1.r 0x05
#define vec1.g 0x06
#define vec1.b 0x07
#define vec1.d 0x08
#define vec1.s 0x09

#define vec2.r 0x0A
#define vec2.g 0x0B
#define vec2.b 0x0C
#define vec2.d 0x0D
#define vec2.s 0x0E

#define vec3.r 0x0F
#define vec3.g 0x10
#define vec3.b 0x11
#define vec3.d 0x12
#define vec3.s 0x13

#define vec4.r 0x14
#define vec4.g 0x15
#define vec4.b 0x16
#define vec4.d 0x17
#define vec4.s 0x18

#define vec5.r 0x19
#define vec5.g 0x1A
#define vec5.b 0x1B
#define vec5.d 0x1C
#define vec5.s 0x1D

#define vec6.r 0x1E
#define vec6.g 0x1F
#define vec6.b 0x20
#define vec6.d 0x21
#define vec6.s 0x22

#define vec7.r 0x23
#define vec7.g 0x24
#define vec7.b 0x25
#define vec7.d 0x26
#define vec7.s 0x27

#define random 42
#define indirectAddress 40
#define indirectData 41
#define returnAddress 45

RST 1
LDC 5
STR vec0.s
STR vec1.s
STR vec2.s
STR vec3.s
STR vec4.s
STR vec5.s
STR vec6.s
STR vec7.s


LDC 0
STR vec0.d
LDC -32
STR vec1.d
LDC -64
STR vec2.d
LDC -96
STR vec3.d
LDC -128
STR vec4.d
LDC 32
STR vec5.d
LDC 64
STR vec6.d
LDC 96
STR vec7.d


LDC 67
STR random ;set random seed
LDR random

:loop

LDC ret0
STR returnAddress
LDC vec0.r
STR indirectAddress
JMP randColor
:ret0



LDC ret1
STR returnAddress
LDC vec1.r
STR indirectAddress
JMP randColor
:ret1

LDC ret2
STR returnAddress
LDC vec2.r
STR indirectAddress
JMP randColor
:ret2

LDC ret3
STR returnAddress
LDC vec3.r
STR indirectAddress
JMP randColor
:ret3

LDC ret4
STR returnAddress
LDC vec4.r
STR indirectAddress
JMP randColor
:ret4

LDC ret5
STR returnAddress
LDC vec5.r
STR indirectAddress
JMP randColor
:ret5

LDC ret6
STR returnAddress
LDC vec6.r
STR indirectAddress
JMP randColor
:ret6

LDC ret7
STR returnAddress
LDC vec7.r
STR indirectAddress
JMP randColor
:ret7




INC vec0.d
INC vec1.d
INC vec2.d
INC vec3.d
INC vec4.d
INC vec5.d
INC vec6.d
INC vec7.d
INC vec0.d
INC vec1.d
INC vec2.d
INC vec3.d
INC vec4.d
INC vec5.d
INC vec6.d
INC vec7.d


UDC 2
JMP loop


:randColor

LDC 0x0F
AND:W random
ADD:R indirectData
INC indirectAddress

LDC 0x0F
AND:W random
ADD:R indirectData
INC indirectAddress

LDC 0x0F
AND:W random
ADD:R indirectData


RET


