#ifndef VECTORS_H_INCLUDED
#define VECTORS_H_INCLUDED

#include <avr/io.h>

typedef struct _led
{
    uint8_t r, g, b;
    int8_t direction;
} led;

typedef struct _vec
{
    uint8_t r, g, b;
    int8_t direction;
    uint8_t sector;
} vec;


void updateLed(led& current_led, vec* vectors, uint8_t size);


#endif // VECTORS_H_INCLUDED
