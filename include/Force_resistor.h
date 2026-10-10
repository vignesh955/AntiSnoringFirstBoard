#ifndef FORCE_RESISTOR_H
#define FORCE_RESISTOR_H

#include <Arduino.h>



void forceInit();
float getAverageReading(int8_t channel);
float unitsToGrams(int units);
int findValveNumber();
float sensorReading(int8_t valveNumber);


#endif