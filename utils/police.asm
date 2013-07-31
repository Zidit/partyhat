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

#define r1 0x80
#define r2 0x81

RST 1
LDC 64
STR vec0.d
LDC -64
STR vec1.d
LDC 6
STR vec0.s
STR vec1.s

LDC 5
STR r1
LDC 0
STR vec0.r
LDC 255
STR vec1.b

:main
UDC 1

LDR r1
ADD:R vec0.r

LDC 0xFF
XOR:W r1
STR r2
LDC 1
ADD:W r2

ADD:R vec1.b

LDC 0x80
SIWS r1
JMP pos

LDC 6
SIWB vec0.r
JMP main
JMP swap


:pos
LDC 0xF9
SIWS vec0.r
JMP main
JMP swap


:swap
LDC 0xFF
XOR:R r1
LDC 1
ADD:R r1

JMP main
