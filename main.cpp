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

uint16_t anim[] = {	0x01FF, 0x0300, 0x0106, 0x0304, 0x0100, 0x0303, 0x0904, 0x1105, 0x1506 };

uint16_t anim2[] = {	0x0100, 0x0303, 0x0153, 0x0308, 0x01AB, 0x030D, 0x0106, 0x0304, 0x0309, 0x030E, 0x01FF, 0x0300, 0x0306, 0x030C, 
						0x0903, 0x0908, 0x090D, 0x0908, 0x090D, 0x090D, 0x1100, 0x150E};



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


    char data[64];
    uint8_t dataLength = 0;
    uint8_t recivedBytes = 0;

    while(1){
        if(xbee.dataAvailable())
        {
            char c = xbee.getChar();

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
            }
        }


        // Echo debug characters
        if(debug.dataAvailable())
        {


        }



        if( taskManager::getTimeMs() >= nextFrame)
        {
            uint32_t time = taskManager::getTimeMs();

			nextFrame = time + 10 * runAnimationCode(anim2);


            for (int ledIndex = 0; ledIndex < number_of_leds; ledIndex++)
            {
                updateLed(leds[ledIndex], vectors, NUMBER_OF_VECTORS);
                gammaCorrection(leds[ledIndex]);
            }

            userImplementationUpdate();
        }



        /// main loop:
        /// -get data from xbee
        /// -prosess data
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




