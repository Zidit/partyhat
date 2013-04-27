#ifndef BATTERY_H_INCLUDED
#define BATTERY_H_INCLUDED

#include <avr/io.h>

namespace batteryManager {

void initialize();
void readLevel();
void highCurrentCharging(bool mode);

uint16_t getBatteryLevel();


}
#endif // BATTERY_H_INCLUDED
