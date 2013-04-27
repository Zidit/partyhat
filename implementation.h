#ifndef IMPLEMENTATION_H_INCLUDED
#define IMPLEMENTATION_H_INCLUDED

#include "vectors.h"


extern const uint8_t number_of_leds;
extern led leds[];

void userImplementationSetup();
void userImplementationUpdate();
void userImplementationCommunication(char* data, uint8_t len);


#endif // IMPLEMENTATION_H_INCLUDED
