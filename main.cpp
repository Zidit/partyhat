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

#include "button.h"
#include "pgm_animations.h"

#include "animations.h"
#include "animation_tasks.h"

button b_change_animation(&PORTA, PIN2);


uart xbeeSerial(&USARTD0, 57600);
ISR (USARTD0_RXC_vect){ xbeeSerial.rxInterrupt(); }
ISR (USARTD0_DRE_vect){ xbeeSerial.txInterrupt(); }

uart debug(&USARTC1, 57600);
ISR (USARTC1_RXC_vect){ debug.rxInterrupt(); }
ISR (USARTC1_DRE_vect){ debug.txInterrupt(); }

uint16_t animationBuffer1[256];

uint16_t animationBuffer2[256];

uint16_t* currentAnimation = animationBuffer1;
uint16_t* nextAnimation = animationBuffer2;

vec vectors[NUMBER_OF_VECTORS];

void test_debug();
void gammaCorrection(led& currentLed);
void brightness(led& currentLed, uint8_t value);
void xbee_api_callback(ZBRxResponse &rx);

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();


bool helsinkiMode = false;

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


    debug.sendStringPgm(PSTR("\n\n\nPartyhat version 0.1 \n"));


    Stream xbeeStream(xbeeSerial);
    xbee.begin(xbeeStream);

    PORTD.DIRSET = PIN4_bm;
    PORTD.OUTCLR = PIN4_bm;
    _delay_ms(20);
    PORTD.OUTSET = PIN4_bm;

	loadPgmAnimation(currentAnimation, anim_spectrum);

    uint32_t nextFrame = taskManager::getTimeMs() ;

	load_nth_animation(1);
	anim_runner.start_animation();

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

			if(!helsinkiMode) 
			{
				nextFrame = time + 10 * runAnimationCode(currentAnimation);
			}
			else {
				anim_runner.run(0);
				nextFrame = anim_runner.getNextRunTime();
			}


            for (int ledIndex = 0; ledIndex < number_of_leds; ledIndex++)
            {
                updateLed(leds[ledIndex], vectors, NUMBER_OF_VECTORS);
				brightness(leds[ledIndex], 255);
                gammaCorrection(leds[ledIndex]);
            }

            userImplementationUpdate();

	}

	if(b_change_animation.isToggled() && b_change_animation.isDown())
	{
		helsinkiMode = false;

		loadNextAnimation(nextAnimation);

		//swap buffers
		uint16_t* ptr16;
		ptr16 = currentAnimation;
		currentAnimation = nextAnimation;
		nextAnimation = ptr16;
	
		resetPC();		
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

void brightness(led& currentLed, uint8_t value)
{
        currentLed.r = ((uint16_t)currentLed.r * (uint16_t)value) >> 8;
        currentLed.g = ((uint16_t)currentLed.g * (uint16_t)value) >> 8;
        currentLed.b = ((uint16_t)currentLed.b * (uint16_t)value) >> 8;

}

void xbeeEcho(ZBRxResponse &rx)
{

	XBeeAddress64 address =  rx.getRemoteAddress64();

	// Create a TX Request
	ZBTxRequest zbTx = ZBTxRequest(address, rx.getData(), rx.getDataLength());

	// Send your request
	xbee.send(zbTx);  
}


void xbee_api_callback(ZBRxResponse &rx)
{
	uint16_t* ptr16;
	uint8_t* ptr8;
	uint16_t k,i;
	uint8_t offset;

    // Check first byte
    switch(rx.getData(0))
    {
        // TODO: Define a sane baseline protocol, for testing we have this dummy RGB setter
        case 0x00:
  
            debug.sendHex(rx.getData(1));
            debug.sendHex(rx.getData(2));
            debug.sendHex(rx.getData(3));

            break;

        case 0x2:
        {
            load_nth_animation(rx.getData(1));
            break;
        }
        case 0x3:
        {
			helsinkiMode = true;
            anim_runner.start_animation();
            break;
        }
        case 0x4:
        {
            anim_runner.stop_animation();
            break;
        } 
	    case 0x5:
        {
			helsinkiMode = true;
            load_nth_animation(rx.getData(1));
            anim_runner.start_animation();
            break;
        }

        case 0x1:
   
            break;
        
        case 0x58: // Ascii X
        
            // Your extended protocol goes here
            break;
        
		case 0x70: 
			xbeeEcho(rx);
            break;  
    
		case 0x80:	
			helsinkiMode = false;
			//swap buffers
			ptr16 = currentAnimation;
			currentAnimation = nextAnimation;
			nextAnimation = ptr16;
			
			resetPC();
			break;

		case 0x81:
		
			offset = rx.getData(1) * 32;
			ptr8 = rx.getData() + 2;
			i = 0;
			k = 0;
			while(i < rx.getDataLength() - 2)
			{
				uint16_t val = ptr8[i++];
				val <<= 8;
				val += (uint16_t)ptr8[i++] & 0x00FF;

				nextAnimation[k + offset] = val;					
				k++;
			}		
			break;
	
		case 0x82:
			
			for(i = 0; i < 256; i++)
			{			
				debug.sendHex(currentAnimation[i]);
				debug.sendChar(' ');		
			}
			break;
		case 0x83:
			
			for(i = 0; i < 256; i++)
			{			
				nextAnimation[i] = 0;	
			}
			break;

    }
}




