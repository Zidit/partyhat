/*
 */


#include <avr/io.h>

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



void test_debug();

void gammaCorrection(led& currentLed);
void processData(char* data, uint8_t len);

vec vectors[NUMBER_OF_VECTORS];

uint32_t nextFrame;

int main(void)
{
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
    selectAnimation(1);

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
            char c;
            c = debug.getChar();
            if( c == '0') selectAnimation(0);
            if( c == '1') selectAnimation(1);
            if( c == '2') selectAnimation(2);
            if( c == '3') selectAnimation(3);
            if( c == '4') selectAnimation(4);
            debug.sendChar(c);

        }



        if( taskManager::getTimeMs() >= nextFrame)
        {
            uint32_t time = taskManager::getTimeMs();
            nextFrame = time + 20;

            updateAnimation(time);

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
        selectAnimation(data[1]);
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
