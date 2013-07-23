Partyhat
========
Alternative code for mesh networked partyhats.
See orginal version and hardware at https://github.com/HelsinkiHacklab/partyhatwork
This version won't work with orginal firmware.


## Why alternative version
Orginal partyhat version has one flaw. It doesn't scale well with large ammounts of
led. This verion is will work any number of leds. It is [tested](http://www.youtube.com/watch?v=SGMewqFb5E4) succesfuly with 52 leds.
Programming gives also easier accsess to different module. All data (time, battery level,
sound level, etc.) can be palaced to "memory", where it can be used on animation program.


## Programing animations
Animations are programmed dynamicly to hats with PAssembly (Party assembly). PAssembly has 26 basic instucion,
one working register (same as pic microcontrollers), 256 bytes "ram", indirect addresssing,
max code size of 256 instructions and carry bit.

All logical and aritmetic operations are made between w-register and specified ram loaction. 
Result can be stored to w-register or to the same ram location. All instructions are expained
in assembler folder.

Ram is divided in half. Upper 128 byte (0x80 - 0xFF) are general purpose memory. Programmer can use 
these as he likes. Lower 128 bytes (0x00 - 0x7F) are special memory loctions. With these program can
access different things like indirect address, battery level, uptime, random generator and vectors (exlpaned
later). More details can also be found in assembler folder.

## Vectors
The point of assembly program is to modify one or more "vectors". Vectors are used to calculte the final
color for each led. Each vector have rgb color, direction and sector values. Color of the led is calculated
by comparing leds direction with vectors direction and sector. Leds color is lineary calculated by it's position
in sector. Final color of led is calculted by adding each vectors effect together. 

Direction is signed byte value. It divides hat to 256 decrees. 0 is front, -128 is back, 64 is right and -64  is to right.
Sector will define how wide vector influences. It can be value between 0 - 7. 0 means it's doesn't do anything. 
Else the sector is power of two of the value to both side of the vectors direction. So value of 1 will give sector 
of 2. 2 will give 8 and so on. Value of 7 will give full sector if 256, so led in same direction as vector will be full
bright and brgihtness will fade to zero on other side.







