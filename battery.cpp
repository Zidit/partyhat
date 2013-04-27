
#include "battery.h"
#include "utils.h"

const uint8_t ADC_CH_MUXPOS_PIN9_gc = (0x09<<3); /* Input pin B1 */

uint16_t batteryLevel;
mean<uint16_t, 8> roundADC;


namespace batteryManager {


void initialize(){
	ADCA.CTRLA |= ADC_ENABLE_bm;                                   	// enable adc
	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc ;                          // 12 bit unsigned conversion
	ADCA.REFCTRL = 0x40;           									// Vcc/2 reference
	ADCA.PRESCALER = ADC_PRESCALER_DIV256_gc;             			// peripheral clk/256 (32MHz/256=125kHz)
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN9_gc ;                      // pos = B1
}


void readLevel(){
    ADCA.CH0.CTRL |= 0x80;
    while (!(ADCA.CH0.INTFLAGS & 0x01));

    int32_t level = roundADC.calc(ADCA.CH0.RES);

    //battery level = 3 * RES * (Vcc /2 ) / 4096 - dV , [ dV = (Vcc /2) * 0.05 ]
    //battery level = 3 * (RES * 1.65 / 4096 - 0.085)
    //battery level mV = RES * 1.2085 - 255
    level = level * 1237 - 225000;//261120;
    batteryLevel = level >> 10;

}

uint16_t getBatteryLevel(){
    return batteryLevel;
}

void highCurrentCharging(bool mode){

    if (mode) PORTB.DIRSET = PIN0_bm;
    else PORTB.DIRCLR = PIN0_bm;

}

}


