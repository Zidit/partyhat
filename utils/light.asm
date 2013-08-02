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



RST 1
LDC 64
STR vec0.d
LDC -64
STR vec1.d
LDC 7
STR vec0.s
STR vec1.s

LDC 255
STR vec0.r
STR vec0.g
STR vec0.b
STR vec1.r
STR vec1.g
STR vec1.b

:main
UDC 10


JMP main
