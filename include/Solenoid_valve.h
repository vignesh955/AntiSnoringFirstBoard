#ifndef SOLENOID_VALVE_H
#define SOLENOID_VALVE_H

#include <Arduino.h>
#define VALVE_1 6 // SENSOR_1
#define VALVE_2 7
#define VALVE_3 15
#define VALVE_4 16
#define VALVE_5 42  // MASTER AIR BAG
#define DEFLATION 1 // DEFLATION VALVE


void solenoidValveInit();
void turnOnAllValves();
void turnOffAllValves();
void solenoidValveOn(int valveNumber);
void solenoidValveOff(int valveNumber);
#endif