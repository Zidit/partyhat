#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


#include "serial.h"
#include "vectors.h"


//Make serial connections global
extern uart debug;
extern uart xbeeSerial;


extern vec vectors[];



#define NUMBER_OF_VECTORS 8
#define ARDUINO_WRAPPER




#endif // CONFIG_H_INCLUDED
