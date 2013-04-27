

#include "vectors.h"


inline int8_t abs8(int8_t x)
{
    return (x < 0 ? -x : x);
}


void vectorToRGB(vec& vector, led& current_led)
{

    int8_t angle = abs8(vector.direction - current_led.direction);
    if(angle >= (1 << vector.sector) || angle == -128 ){
        current_led.r = 0;
        current_led.g = 0;
        current_led.b = 0;
        return;
    }

    uint16_t t = 256 >> vector.sector;
    uint16_t t2 = (1 << vector.sector) - angle;
    uint16_t intesity = t * t2;

    uint16_t red = (uint16_t)vector.r * intesity;
    current_led.r = red >> 8;

    uint16_t green = (uint16_t)vector.g * intesity;
    current_led.g = green >> 8;

    uint16_t blue  = (uint16_t)vector.b * intesity;
    current_led.b = blue >> 8;

}


void updateLed(led& current_led, vec* vectors, uint8_t size)
{
        uint16_t r = 0, g = 0, b = 0;
        led temp = current_led;

        for (int i = 0; i < size; i++) {
            if (vectors[i].sector > 0 && vectors[i].sector < 8) {
                vectorToRGB(vectors[i], temp);
                r += temp.r;
                g += temp.g;
                b += temp.b;
            }
        }

        if(r > 0xFF)
            current_led.r = 0xFF;
        else
            current_led.r = r;

        if(g > 0xFF)
            current_led.g = 0xFF;
        else
            current_led.g = g;

        if(b > 0xFF)
            current_led.b = 0xFF;
        else
            current_led.b = b;

}
