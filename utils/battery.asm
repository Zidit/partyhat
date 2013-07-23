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
#define battery_upper 0x2B
#define battery_lower 0x2C

#define ram_color_r 0x80
#define ram_color_g 0x81
#define ram_color_b 0x82
#define ram_bat_lvl 0x83
#define ram_tmp 0x84



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
LDC -127
STR vec4.d
LDC 96
STR vec5.d
LDC 64
STR vec6.d
LDC 32
STR vec7.d


LDC 23
STR random


:loop


LDR battery_lower
SR 5
STR ram_bat_lvl

LDC 0x1F
AND:W battery_upper
SL 3
OR:R ram_bat_lvl

;DEBUG battery_upper
;DEBUG battery_lower
DEBUG ram_bat_lvl

LDC 0
STR ram_color_r
STR ram_color_g
STR ram_color_b
STR vec6.b

:skip


LDC 0
STR vec0.r
STR vec1.r
STR vec0.g
STR vec1.g
STR vec0.b
STR vec1.b




LDC 0x6E
SIWB ram_bat_lvl
JMP hi

LDC 200
STR ram_color_r
JMP skip3

:hi
LDC 200
STR ram_color_g

:skip3


LDR ram_color_r
STR vec0.r
STR vec1.r


LDR ram_color_b
STR vec0.b
STR vec1.b

LDR ram_color_g
STR vec0.g
STR vec1.g




UDC 5
JMP loop


