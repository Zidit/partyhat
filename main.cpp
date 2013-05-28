/*
 */


#include <avr/io.h>
#include <stdlib.h>

#include "vectors.h"
#include "animation.h"
#include "serial.h"
#include "taskmanager.h"
#include "utils.h"
#include "config.h"
#include "gamma.h"
#include "battery.h"
#include "implementation.h"


uart xbee(&USARTD0, 9600);
ISR (USARTD0_RXC_vect){ xbee.rxInterrupt(); }
ISR (USARTD0_DRE_vect){ xbee.txInterrupt(); }

uart debug(&USARTC1, 9600);
ISR (USARTC1_RXC_vect){ debug.rxInterrupt(); }
ISR (USARTC1_DRE_vect){ debug.txInterrupt(); }

uint16_t defaultAnimation[] = { 0x1801, 0x0107, 0x0304, 0x0309, 0x030e, 0x0100, 0x0303, 0x0155, 0x0308, 0x01ab, 0x030d, 0x01ff, 0x0300, 0x0307, 0x030b, 0x0101, 0x0403, 0x0408, 0x040d, 0x1102, 0x170f};



uint16_t animation[256];



void test_debug();

void gammaCorrection(led& currentLed);
void processData(char* data, uint8_t len);

vec vectors[NUMBER_OF_VECTORS];

uint32_t nextFrame;

int main(void)
{
	srand(12);
    // Set sys clock to 16 Mhz
    // -if you change this, remember to change F_CUP define too
    set32MHzClock(CLK_PSADIV_2_gc);

    //Enable all interrupts
    PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	sei();


    userImplementationSetup();

    batteryManager::initialize();
    batteryManager::highCurrentCharging(false);
    taskManager::initialize();
    taskManager::registerTask(batteryManager::readLevel, 100, 0);
    //taskManager::registerTask(&test_debug, 100, 0);


    debug.sendStringPgm(PSTR("Partyhat version 0.1 \n"));


    nextFrame = taskManager::getTimeMs() ;

    uint8_t dataLength = 0;
    uint8_t recivedBytes = 0;

	uint16_t* currentAnimation = defaultAnimation;

    while(1){
        if(xbee.dataAvailable())
        {
         /*   char c = xbee.getChar();

            if(dataLength == 0) dataLength = c;
            else
            {
                data[recivedBytes] = c;
                recivedBytes++;
            }

            if(dataLength == recivedBytes)
            {
                processData(data, dataLength);
                dataLength = 0;
                recivedBytes = 0;
            }*/
        }


        if(debug.dataAvailable())
        {
            char c = debug.getChar();

            if(dataLength == 0) dataLength = c;
            else
            {
				uint8_t *ptr = (uint8_t*)animation;
				ptr[recivedBytes] = c;
				recivedBytes++;
            }

            if(dataLength == recivedBytes)
            {
				resetPC();
                currentAnimation = animation;
                dataLength = 0;
                recivedBytes = 0;
            }
        }



        if( taskManager::getTimeMs() >= nextFrame)
        {
            uint32_t time = taskManager::getTimeMs();

			nextFrame = time + 10 * runAnimationCode(currentAnimation);

            for (int ledIndex = 0; ledIndex < number_of_leds; ledIndex++)
            {
                updateLed(leds[ledIndex], vectors, NUMBER_OF_VECTORS);
                gammaCorrection(leds[ledIndex]);
            }

            userImplementationUpdate();
        }



        /// main loop:
        /// -get data from xbee
        /// -prosess data							DONE
        /// -update vectors in animation handler    DONE
        /// -get rgb values from vectors            DONE
        /// -gamma correction + ect.                DONE
        /// -set rgb values to all leds             REFERENCE IMPLEMENTATION DONE

        /// Backgound tasks:
        /// -batterymonitor                         NOT TESTED

    }

    return 0;
}


void test_debug()
{

    debug.sendInt(batteryManager::getBatteryLevel());
    debug.sendString(" mV\n\r");

}


void gammaCorrection(led& currentLed)
{
        currentLed.r = gamma256to256(currentLed.r);
        currentLed.g = gamma256to256(currentLed.g);
        currentLed.b = gamma256to256(currentLed.b);
}


void processData(char* data, uint8_t len)
{

    //No data, shouldn't happen
    if(!len) return;

    switch (data[0])
    {

    //echo
    case 0x00:
        for (uint8_t i = 1; i < len; i++)
            xbee.sendChar(data[i]);
        break;

    //Set animation
    case 0x01:
        break;

    //set vectors manualy
    case 0x02:

        break;


    case 0xFF:
        userImplementationCommunication(data + 1, len - 1);
        break;

    default:
        break;
    }

}




