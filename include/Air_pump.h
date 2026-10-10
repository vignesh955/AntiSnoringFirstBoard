#ifndef PUMP_H
#define PUMP_H

#include <Arduino.h>

#define AIRPUMP 17

#define PWM_CHANNEL 0
#define PWM_FREQ 5000    // 5 kHz frequency
#define PWM_RESOLUTION 8 // 8-bit resolution

void airPumpInit();
void airPumpOn();
void airPumpOff();
void setMotorSpeed(int speed);

#endif // PUMP_H
