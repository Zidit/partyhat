
#include "implementation.h"

#include <avr/io.h>

const uint8_t number_of_leds = 52;
led leds[number_of_leds];

void sendData(const uint8_t data);
inline void setLed(const uint8_t value){
	uint8_t val = value >> 1;
	val |=0x80;
	sendData(val);
}


void userImplementationSetup()
{
    //Setup onboard leds

  /*  PORTC.DIRSET = 0b00111111;

    //Inverts are used becouse I fuced up one of the fets
    //In working board these lines should be removed
    PORTC.PIN4CTRL |= PORT_INVEN_bm;
    PORTC.PIN5CTRL |= PORT_INVEN_bm;


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
    TCC1.CTRLA = TC_CLKSEL_DIV64_gc;*/

    // Set dirctions for all rgb leds. This is design specific.

    leds[0].direction = 125;
    leds[1].direction = 120;
    leds[2].direction = 115;
    leds[3].direction = 110;
    leds[4].direction = 105;
    leds[5].direction = 100;
    leds[6].direction = 95;
    leds[7].direction = 90;
    leds[8].direction = 85;
    leds[9].direction = 80;

    leds[10].direction = 75;
    leds[11].direction = 70;
    leds[12].direction = 65;
    leds[13].direction = 60;
    leds[14].direction = 55;
    leds[15].direction = 50;
    leds[16].direction = 45;
    leds[17].direction = 40;
    leds[18].direction = 35;
    leds[19].direction = 30;

    leds[20].direction = 25;
    leds[21].direction = 20;
    leds[22].direction = 15;
    leds[23].direction = 10;
    leds[24].direction = 5;
    leds[25].direction = 0;
    leds[26].direction = -5;
    leds[27].direction = -10;
    leds[28].direction = -15;
    leds[29].direction = -20;

    leds[30].direction = -25;
    leds[31].direction = -30;
    leds[32].direction = -35;
    leds[33].direction = -40;
    leds[34].direction = -45;
    leds[35].direction = -50;
    leds[36].direction = -55;
    leds[37].direction = -60;
    leds[38].direction = -65;
    leds[39].direction = -70;

    leds[40].direction = -75;
    leds[41].direction = -80;
    leds[42].direction = -85;
    leds[43].direction = -90;
    leds[44].direction = -95;
    leds[45].direction = -100;
    leds[46].direction = -105;
    leds[47].direction = -110;
    leds[48].direction = -115;
    leds[49].direction = -120;

    leds[50].direction = -125;
    leds[51].direction = -128;


	PORTA.DIRSET = PIN0_bm | PIN1_bm;
	PORTA.OUTCLR = PIN0_bm | PIN1_bm;
	sendData(0);

}


void userImplementationUpdate()
{

    /*TCC0.CCA = leds[0].b;
    TCC0.CCB = leds[0].g;
    TCC0.CCC = leds[0].r;

    TCC0.CCD = leds[50].b;
    TCC1.CCA = leds[50].r;
    TCC1.CCB = leds[50].g;*/


	for (uint8_t i = 0; i < 52; i++) { 

		setLed(leds[i].b);
		setLed(leds[i].r);
		setLed(leds[i].g);
	}

	sendData(0);
}

void userImplementationCommunication(char* data, uint8_t len)
{





}


void sendData(const uint8_t data)
{

	uint8_t mask = 0x80;

	while (mask){
		if (mask & data)
            PORTA.OUTSET = PIN0_bm;
		else
			PORTA.OUTCLR = PIN0_bm;

		mask >>= 1;

		PORTA.OUTSET = PIN1_bm;
		PORTA.OUTCLR = PIN1_bm;

	}

}
