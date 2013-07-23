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


RST 1
LDC 7
STR vec0.s
STR vec1.s
STR vec2.s

LDC 0
STR vec0.d
LDC 85
STR vec1.d
LDC -85
STR vec2.d

LDC 255
STR vec0.r
STR vec1.b
STR vec2.g

:loop
LDC 2
ADD:R vec0.d
LDC 4
ADD:R vec1.d
LDC 6
ADD:R vec2.d

UDC 2
JMP loop


