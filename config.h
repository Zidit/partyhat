#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


#include "serial.h"
#include "vectors.h"


//Make serial connections global
extern uart debug;
extern uart xbee;


extern vec vectors[];



#define NUMBER_OF_VECTORS 8




#endif // CONFIG_H_INCLUDED
