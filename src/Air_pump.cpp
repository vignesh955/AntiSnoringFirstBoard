#include <Arduino.h>
#include "Air_pump.h"

void setMotorSpeed(int speed)
{
    speed = constrain(speed, 0, 255); // Ensure value is between 0–255
    ledcWrite(PWM_CHANNEL, speed);
}

/**
 * @brief Air pump initialization
 */
void airPumpInit()
{
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(AIRPUMP, PWM_CHANNEL);
}

/**
 * @brief Air pump turn on
 */

void airPumpOn()
{
    setMotorSpeed(0);
    delay(100);
    setMotorSpeed(127); // Maximum - 255
    delay(100);
}

/**
 * @brief Air pump turn off
 */

void airPumpOff()
{
    setMotorSpeed(0);
    delay(100);
}