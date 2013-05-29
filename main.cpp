/*
 */


#include <avr/io.h>
#include <util/delay.h>
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
#include "arduinowrapper.h"
#include "XBee.h"


uart xbeeSerial(&USARTD0, 9600);
ISR (USARTD0_RXC_vect){ xbeeSerial.rxInterrupt(); }
ISR (USARTD0_DRE_vect){ xbeeSerial.txInterrupt(); }

uart debug(&USARTC1, 9600);
ISR (USARTC1_RXC_vect){ debug.rxInterrupt(); }
ISR (USARTC1_DRE_vect){ debug.txInterrupt(); }

uint16_t animationBuffer1[256] = { 0x1801, 0x0107, 0x0304, 0x0309, 0x030e, 0x0100, 0x0303, 0x0155, 
									0x0308, 0x01ab, 0x030d, 0x01ff, 0x0300, 0x0307, 0x030b, 0x0101, 
									0x0403, 0x0408, 0x040d, 0x1102, 0x170f};

uint16_t animationBuffer2[256];

uint16_t* currentAnimation = animationBuffer1;
uint16_t* nextAnimation = animationBuffer2;

vec vectors[NUMBER_OF_VECTORS];

void test_debug();
void gammaCorrection(led& currentLed);
void xbee_api_callback(ZBRxResponse rx);

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();

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


    Stream xbeeStream(xbeeSerial);
    xbee.begin(xbeeStream);

    PORTD.DIRSET = PIN4_bm;
    PORTD.OUTCLR = PIN4_bm;
    _delay_ms(20);
    PORTD.OUTSET = PIN4_bm;

    uint32_t nextFrame = taskManager::getTimeMs() ;


    while(1){

        xbee.readPacket();
        if (xbee.getResponse().isAvailable())
        {
            // got something
            if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
            {
                // got a zb rx packet
                xbee.getResponse().getZBRxResponse(rx);
                xbee_api_callback(rx);
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
        /// -batterymonitor                         DONE

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


void sendHI()
{
		// Create an array for holding the data you want to send.
	uint8_t payload[] = "Hello World!";

	// Specify the address of the remote XBee (this is the SH + SL)
	XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x4089EDE8);

	// Create a TX Request
	ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

	// Send your request
	xbee.send(zbTx);
}

void xbee_api_callback(ZBRxResponse rx)
{
	uint16_t* ptr16;
	uint8_t* ptr8;

    // Check first byte
    debug.sendHex(rx.getData(0));
    switch(rx.getData(0))
    {
        // TODO: Define a sane baseline protocol, for testing we have this dummy RGB setter
        case 0x0:
  
            debug.sendHex(rx.getData(1));
            debug.sendHex(rx.getData(2));
            debug.sendHex(rx.getData(3));

            break;
 
        case 0x1:
        
				resetPC();


            break;
        

		case 0x06:
			sendHI();
			break;
		
        case 0x58: // Ascii X
        
            // Your extended protocol goes here
            break;
        
		case 0x80:	

			//swap buffers
			ptr16 = currentAnimation;
			currentAnimation = nextAnimation;
			nextAnimation = ptr16;
			
			resetPC();
			break;

		case 0x81:
			
			uint8_t offset = rx.getData(1);
			ptr8 = rx.getFrameData() + 2;
			
			for(int i = 0; i < rx.getFrameDataLength() - 2; i++)
			{
				nextAnimation[i + offset]  = ptr8[i * 2] << 8;
				nextAnimation[i + offset] += ptr8[(i * 2) + 1];
			}
			break;


    }
}




