/*
 */


#include <avr/io.h>
#include <util/delay.h>

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



void test_debug();

void gammaCorrection(led& currentLed);
void xbee_api_callback(ZBRxResponse rx);


vec vectors[NUMBER_OF_VECTORS];
bool runAnimation ;

uint32_t nextFrame;

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();

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


//    debug.sendStringPgm(PSTR("Partyhat version 0.1 \n"));

    Stream xbeeStream(xbeeSerial);
    //Stream xbeeStream(debug);

    PORTD.DIRSET = PIN4_bm;
    PORTD.OUTCLR = PIN4_bm;
    _delay_ms(20);
    PORTD.OUTSET = PIN4_bm;

    xbee.begin(xbeeStream);



    nextFrame = taskManager::getTimeMs() ;
    selectAnimation(1);
    runAnimation = true;

    char data[64];
    uint8_t dataLength = 0;
    uint8_t recivedBytes = 0;

    while(1){
        xbee.readPacket();
        if (xbee.getResponse().isAvailable())
        {
            // got something
            if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
            {
				 runAnimation = false;
                // got a zb rx packet
                xbee.getResponse().getZBRxResponse(rx);
                xbee_api_callback(rx);
            }
        }


        // Echo debug characters
        if(debug.dataAvailable())
        {
            char c = debug.getChar();

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



        if( taskManager::getTimeMs() >= nextFrame)
        {
            uint32_t time = taskManager::getTimeMs();
            nextFrame = time + 20;

            if(runAnimation) updateAnimation(time);

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
        /// -update vect"ors in animation handler    DONE
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
    // Check first byte
    debug.sendHex(rx.getData(0));
    switch(rx.getData(0))
    {
        // TODO: Define a sane baseline protocol, for testing we have this dummy RGB setter
        case 0x0:
        {
            TCC0.CCA = rx.getData(3);
            TCC0.CCB = rx.getData(2);
            TCC0.CCC = rx.getData(1);

            debug.sendHex(rx.getData(1));
            debug.sendHex(rx.getData(2));
            debug.sendHex(rx.getData(3));

            break;
        }
        case 0x1:
        {

            TCC0.CCD = rx.getData(3);
            TCC1.CCA = rx.getData(1);
            TCC1.CCB = rx.getData(2);
            break;
        }
        case 0x2:
        {
            selectAnimation(rx.getData(1));
            break;
        }
        case 0x3:
        {
            runAnimation = true;
            break;
        }
        case 0x4:
        {
            runAnimation = false;
            break;
        }
        case 0x5:
        {
            selectAnimation(rx.getData(1));
            runAnimation = true;
            break;
        }
		case 0x06:
			sendHI();
			break;
		
        case 0x58: // Ascii X
        {
            // Your extended protocol goes here
            break;
        }
    }
}
