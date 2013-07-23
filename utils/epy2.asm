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
LDC 7
STR vec0.s
STR vec1.s


LDC 0
STR vec0.d
LDC -128
STR vec1.d



LDC 67
STR random ;set random seed


:loop

LDR random 
STR vec0.r
STR vec0.g
STR vec0.b
STR vec1.r
STR vec1.g
STR vec1.b

UDC 10
JMP loop




