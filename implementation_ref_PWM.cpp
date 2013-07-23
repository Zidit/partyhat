
#include "implementation.h"

#include <avr/io.h>

const uint8_t number_of_leds = 2;
led leds[number_of_leds];



void userImplementationSetup()
{
    //Setup onboard leds

    PORTC.DIRSET = 0b00111111;

    TCC0.PER = 0x00FF;
    TCC0.CCA = 0;
    TCC0.CCB = 0;
    TCC0.CCC = 0;
    TCC0.CCD = 0;
    TCC0.CTRLB = TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm | TC_WGMODE_DS_B_gc;
    TCC0.CTRLA = TC_CLKSEL_DIV64_gc;

    TCC1.PER = 0x00FF;
    TCC1.CCA = 0;
    TCC1.CCB = 0;
    TCC1.CTRLB = TC1_CCAEN_bm | TC1_CCBEN_bm | TC_WGMODE_DS_B_gc;
    TCC1.CTRLA = TC_CLKSEL_DIV64_gc;

    // Set dirctions for all rgb leds. This is design specific.

    leds[0].direction = 0;
    leds[1].direction = -128;
 

}


void userImplementationUpdate()
{

    TCC0.CCA = leds[0].b;
    TCC0.CCB = leds[0].g;
    TCC0.CCC = leds[0].r;

    TCC0.CCD = leds[50].b;
    TCC1.CCA = leds[50].r;
    TCC1.CCB = leds[50].g;

}

void userImplementationCommunication(char* data, uint8_t len)
{





}


