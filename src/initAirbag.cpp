#include <Arduino.h>
#include "Solenoid_valve.h"
#include "Piezo_senosr.h"
#include "watchDog.h"
#include "initAirbag.h"

float initAirpressure[5];

void initAirbagPressure()
{
    // Master airbag initial pressure calculation
    turnOffAllValves();

    solenoidValveOn(5); // Master airbag
    solenoidValveOn(6); // Deflation valve

    uint32_t timeOut = millis();
    uint32_t watchdogTimeout = millis();
    while (millis() - timeOut < 15000)
    {

        if (millis() - watchdogTimeout > 2000)
        {
            feedWatchdog();
            watchdogTimeout = millis();
        }
    }

    solenoidValveOff(6);

    float pressureReading = piezoSensorReading();

    initAirpressure[0] = pressureReading;
    solenoidValveOff(5);

    // Valve - 1
    turnOffAllValves();

    solenoidValveOn(1); //  
    solenoidValveOn(6); // Deflation valve

    timeOut = millis();
    watchdogTimeout = millis();
    while (millis() - timeOut < 15000)
    {

        if (millis() - watchdogTimeout > 2000)
        {
            feedWatchdog();
            watchdogTimeout = millis();
        }
    }

    solenoidValveOff(6);

    pressureReading = piezoSensorReading();

    initAirpressure[1] = pressureReading;
    solenoidValveOff(1);

    // valve 2

    turnOffAllValves();

    solenoidValveOn(2); //  
    solenoidValveOn(6); // Deflation valve

    timeOut = millis();
    watchdogTimeout = millis();
    while (millis() - timeOut < 15000)
    {

        if (millis() - watchdogTimeout > 2000)
        {
            feedWatchdog();
            watchdogTimeout = millis();
        }
    }

    solenoidValveOff(6);

    pressureReading = piezoSensorReading();

    initAirpressure[2] = pressureReading;
    solenoidValveOff(2);

    // Valve 3
    turnOffAllValves();

    solenoidValveOn(3); //  
    solenoidValveOn(6); // Deflation valve

    timeOut = millis();
    watchdogTimeout = millis();
    while (millis() - timeOut < 15000)
    {

        if (millis() - watchdogTimeout > 2000)
        {
            feedWatchdog();
            watchdogTimeout = millis();
        }
    }

    solenoidValveOff(6);

    pressureReading = piezoSensorReading();

    initAirpressure[3] = pressureReading;
    solenoidValveOff(3);

    // Valve 4
    turnOffAllValves();

    solenoidValveOn(4); //  
    solenoidValveOn(6); // Deflation valve

    timeOut = millis();
    watchdogTimeout = millis();
    while (millis() - timeOut < 15000)
    {

        if (millis() - watchdogTimeout > 2000)
        {
            feedWatchdog();
            watchdogTimeout = millis();
        }
    }

    solenoidValveOff(6);

    pressureReading = piezoSensorReading();

    initAirpressure[4] = pressureReading;
    solenoidValveOff(4);

    for (int i = 0; i < 5; i++)
    {
        Serial.printf("pressure value[%d] : %.2f\n", i, initAirpressure[i]);
    }
}