#include <Arduino.h>
#include "actionPhase.h"
#include "Air_pump.h"
#include "Solenoid_valve.h"
#include "state.h"
#include "flag.h"
#include "Force_resistor.h"
#include "Piezo_senosr.h"
#include "ble.h"
#include <ArduinoJson.h>
#include "watchDog.h"
#include "initAirbag.h"

int valveNumber;
uint32_t lastWatchdogReset;
bool phase1DoneFlag = false;

// Function to calculate percentages based on adjacent sensors
int checkPhase(float w1, float w2, float w3, float w4)
{
    float adj_w1 = w1, adj_w2 = w2, adj_w3 = w3, adj_w4 = w4;

    if (w1 >= w2 && w1 >= w3 && w1 >= w4)
    {
        
        float mini = min(adj_w2, min(adj_w3, adj_w4));
        mini = abs(mini);
        adj_w2 += mini;
        adj_w3 += mini;
        adj_w4 += mini;
        adj_w1 += mini;
    }
    else if (w2 >= w1 && w2 >= w3 && w2 >= w4)
    {
        
        float mini = min(adj_w1, min(adj_w3, adj_w4));
        mini = abs(mini);
        adj_w2 += mini;
        adj_w3 += mini;
        adj_w4 += mini;
        adj_w1 += mini;
    }
    else if (w3 >= w1 && w3 >= w2 && w3 >= w4)
    {
        
        float mini = min(adj_w2, min(adj_w1, adj_w4));
        mini = abs(mini);
        adj_w2 += mini;
        adj_w3 += mini;
        adj_w4 += mini;
        adj_w1 += mini;
    }
    else if (w4 >= w1 && w4 >= w2 && w4 >= w3)
    {
        
        float mini = min(adj_w3, min(adj_w1, adj_w2));
        mini = abs(mini);
        adj_w3 += mini;
        adj_w4 += mini;
        adj_w2 += mini;
        adj_w1 += mini;
    }

    float totalWeight = adj_w1 + adj_w2 + adj_w3 + adj_w4;

    if (totalWeight == 0)
    {
        Serial.println("No valid weights detected");
        // return;
    }

    float p1 = (adj_w1 / totalWeight) * 100;
    float p2 = (adj_w2 / totalWeight) * 100;
    float p3 = (adj_w3 / totalWeight) * 100;
    float p4 = (adj_w4 / totalWeight) * 100;

    Serial.print("Percentage 1: ");
    Serial.print(p1);
    Serial.println("%");
    Serial.print("Percentage 2: ");
    Serial.print(p2);
    Serial.println("%");
    Serial.print("Percentage 3: ");
    Serial.print(p3);
    Serial.println("%");
    Serial.print("Percentage 4: ");
    Serial.print(p4);
    Serial.println("%");

    // Phase detection logic
    if ((p1 >= p2 + 35 && p1 >= p3 + 35 && p1 >= p4 + 35) ||
        (p2 >= p1 + 35 && p2 >= p3 + 35 && p2 >= p4 + 35) ||
        (p3 >= p1 + 35 && p3 >= p2 + 35 && p3 >= p4 + 35) ||
        (p4 >= p1 + 35 && p4 >= p2 + 35 && p4 >= p3 + 35)) 
    {
        Serial.println("Phase 1");
        return 1;
    }
    else 
    {
        Serial.println("Phase 2");
        return 2;
    }

    Serial.println();
}

/**
 * @brief BLE Notify
 */
void actionDefault()
{
    Serial.println("Current State: DEFAULT");

    static float lastReadingSensor1 = -50000;
    static float lastReadingSensor2 = -50000;
    static float lastReadingSensor3 = -50000;
    static float lastReadingSensor4 = -50000;

    float currentReadingSensor4 = sensorReading(0);
    float currentReadingSensor3 = sensorReading(1);
    float currentReadingSensor2 = sensorReading(2);
    float currentReadingSensor1 = sensorReading(3);

    if ((abs(currentReadingSensor1 - lastReadingSensor1) > 500) ||
        (abs(currentReadingSensor2 - lastReadingSensor2) > 500) ||
        (abs(currentReadingSensor3 - lastReadingSensor3) > 500) ||
        (abs(currentReadingSensor4 - lastReadingSensor4) > 500))
    {

        // ble send part
        JsonDocument doc;
        doc["S1"] = currentReadingSensor1;
        doc["S2"] = currentReadingSensor2;
        doc["S3"] = currentReadingSensor3;
        doc["S4"] = currentReadingSensor4;

        String jsonString;
        serializeJson(doc, jsonString);

        Serial.print("JSON created in sensor.cpp: ");
        Serial.println(jsonString);

        // Send JSON over BLE
        sendBleMessage(jsonString);

        lastReadingSensor1 = currentReadingSensor1;
        lastReadingSensor2 = currentReadingSensor2;
        lastReadingSensor3 = currentReadingSensor3;
        lastReadingSensor4 = currentReadingSensor4;
    }

    delay(200);
}

/**
 * @brief Solenoid valve control and Pressure sensor reading corresponding to Phase-1
 */

void actionPhase1()
{
    flag = 1;


    float adc0, adc1, adc2, adc3;

    // Read sensor values
    //  check for sensor number
    adc0 = getAverageReading(3);
    adc1 = getAverageReading(2);
    adc2 = getAverageReading(1);
    adc3 = getAverageReading(0);

    // Convert ADC values to weight
    float w1 = unitsToGrams(adc0);
    float w2 = unitsToGrams(adc1);
    float w3 = unitsToGrams(adc2);
    float w4 = unitsToGrams(adc3);

    
// Ensure miniimum values are -12000
    if (w1 < -12000)
        w1 = -12000;
    if (w2 < -12000)
        w2 = -12000;
    if (w3 < -12000)
        w3 = -12000;
    if (w4 < -12000)
        w4 = -12000;

    int phaseDetect = checkPhase(w1, w2, w3, w4);

    delay(100);

    if (w1 < -200 && w2 < -200 && w3 < -200 && w4 < -200)
    {
        flag = 0;
        currentState = STATE_DEFAULT;
        sendBleStringMessage("No Phase action");
        return;
    }
    else if (phaseDetect == 1)
    {
        phase1DoneFlag = true;

        Serial.println("Current state : phase1");

        solenoidValveOn(5);
        delay(100);
        airPumpOn();
        delay(100);

        float pressureReading = piezoSensorReading();
        uint32_t timeOut = millis();

        lastWatchdogReset = millis();
        while (pressureReading <= initAirpressure[0] + 3.0) 
        {
            pressureReading = piezoSensorReading();
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        Serial.println("phase 1 after Airbag filling ");

        airPumpOff();
        solenoidValveOff(5);
        delay(100);

        sendBleStringMessage("WoN-P1 inflated");

        flag = 0;

        Serial.println("Waiting for BLE data");

        timeOut = millis();                
        while (millis() - timeOut < 15000) 
        {
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        timeOut = millis();               
        while (millis() - timeOut < 5000) 
        {
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        if (currentState == STATE_PHASE1)
        {
            solenoidValveOn(5);
            solenoidValveOn(6);

            phase1DoneFlag = false;

            delay(10);

            Serial.println("deflate phase 1 started");

            pressureReading = piezoSensorReading();
            timeOut = millis();
            while (pressureReading > initAirpressure[0]) 
            {
                pressureReading = piezoSensorReading();
                if (millis() - lastWatchdogReset > 2000)
                {
                    feedWatchdog();
                    lastWatchdogReset = millis();
                }
            }

            // 10 seconds waiting after reaching the initial pressure value and before closing the airbag valve
            timeOut = millis();
            while (millis() - timeOut < 10000) // wait for 10 seconds
            {
                if (millis() - lastWatchdogReset > 2000)
                {
                    feedWatchdog();
                    lastWatchdogReset = millis();
                }
            }

            solenoidValveOff(5);
            solenoidValveOff(6);
            delay(200);

            Serial.println("deflate phase 1 end");
            sendBleStringMessage("WoN-P1 deflated");

            currentState = STATE_DEFAULT;
           
        }
    }

    else if (phaseDetect == 2)
    {
        currentState = STATE_PHASE2;
        actionPhase2();
        return;
    }
    else
    {
        flag = 0;
        currentState = STATE_DEFAULT;
        sendBleStringMessage("No Phase action");
        return;
    }
}

/**
 * @brief Solenoid valve control and Pressure sensor reading corresponding to Phase-2
 */
void actionPhase2()
{
    flag = 1;
    Serial.println("Current state : phase2");

    valveNumber = findValveNumber();

    Serial.println(valveNumber);

    solenoidValveOn(valveNumber);
    delay(100);
    airPumpOn();
    delay(100);

    float pressureReading = piezoSensorReading();
    uint32_t timeOut = millis();
    //
    while (pressureReading <= initAirpressure[valveNumber] + 6) 
    {
        pressureReading = piezoSensorReading();
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    Serial.println("phase 2 after airbag filling ");

    airPumpOff();
    solenoidValveOff(valveNumber);
    delay(100);

    sendBleStringMessage("WoN-P2 inflated");

    flag = 0;

    Serial.println("Waiting for BLE data");

    timeOut = millis();
    while (millis() - timeOut < 15000) 
    {
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    timeOut = millis();               
    while (millis() - timeOut < 5000) 
    {
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    if (currentState == STATE_PHASE2)
    {
        Serial.println("deflate phase 2 Started");

        solenoidValveOn(valveNumber);
        solenoidValveOn(6); // corresponding to deflation valve
        delay(200);

        pressureReading = piezoSensorReading();
        timeOut = millis();
        while (pressureReading > initAirpressure[valveNumber]) 
        {
            pressureReading = piezoSensorReading();
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        // 10 seconds waiting after reaching the initial pressure value and before closing the airbag valve
        timeOut = millis();
        while (millis() - timeOut < 10000) 
        {
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        solenoidValveOff(valveNumber);
        solenoidValveOff(6);

        if (phase1DoneFlag == true)
        {

            phase1DoneFlag = false;

            solenoidValveOn(5);
            solenoidValveOn(6);

            delay(10);

            pressureReading = piezoSensorReading();
            timeOut = millis();
            while (pressureReading > initAirpressure[0]) 
            {
                pressureReading = piezoSensorReading();
                if (millis() - lastWatchdogReset > 2000)
                {
                    feedWatchdog();
                    lastWatchdogReset = millis();
                }
            }

            // 10 seconds waiting after reaching the initial pressure value and before closing the airbag valve
            timeOut = millis();
            while (millis() - timeOut < 10000) 
            {
                if (millis() - lastWatchdogReset > 2000)
                {
                    feedWatchdog();
                    lastWatchdogReset = millis();
                }
            }

            solenoidValveOff(5);
            solenoidValveOff(6);
            delay(10);
        }

        Serial.println("deflate phase 2 end");
        sendBleStringMessage("WoN-P2 deflated");

        currentState = STATE_DEFAULT;
    }
}

/**
 * @brief Solenoid valve control and Pressure sensor reading corresponding to Phase-3
 */

void actionPhase3()
{
    flag = 1;
    Serial.println("Current state : phase3");

    solenoidValveOn(valveNumber);
    airPumpOn();
    delay(100);

    float pressureReading = piezoSensorReading();
    uint32_t timeOut = millis();
    while (pressureReading <= initAirpressure[valveNumber] + 8) 
    {
        pressureReading = piezoSensorReading();
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    Serial.println("phase 3 after airbag filling ");

    airPumpOff();
    solenoidValveOff(valveNumber);
    delay(100);

    sendBleStringMessage("WoN-P3 inflated");

    timeOut = millis();
    while (millis() - timeOut < 5000) 
    {
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    Serial.println("deflate phase 3 started");

    solenoidValveOn(valveNumber);
    solenoidValveOn(6); // corresponding to deflation valve
    delay(10);

    pressureReading = piezoSensorReading();
    timeOut = millis();
    while (pressureReading > initAirpressure[valveNumber]) 
    {
        pressureReading = piezoSensorReading();
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    // 10 seconds waiting after reaching the initial pressure value and before closing the airbag valve
    timeOut = millis();
    while (millis() - timeOut < 10000) 
    {
        if (millis() - lastWatchdogReset > 2000)
        {
            feedWatchdog();
            lastWatchdogReset = millis();
        }
    }

    solenoidValveOff(valveNumber);
    solenoidValveOff(6);

    if (phase1DoneFlag == true)
    {

        phase1DoneFlag = false;

        solenoidValveOn(5);
        solenoidValveOn(6);
        delay(10);

        pressureReading = piezoSensorReading();
        timeOut = millis();
        while (pressureReading > initAirpressure[0]) 
        {
            pressureReading = piezoSensorReading();
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        // 10 seconds waiting after reaching the initial pressure value and before closing the airbag valve
        timeOut = millis();
        while (millis() - timeOut < 10000) 
        {
            if (millis() - lastWatchdogReset > 2000)
            {
                feedWatchdog();
                lastWatchdogReset = millis();
            }
        }

        solenoidValveOff(5);
        solenoidValveOff(6);
        delay(10);
    }

    Serial.println("deflate phase 3 end");
    sendBleStringMessage("WoN-P3 deflated");

    currentState = STATE_DEFAULT;
    delay(100);
    

    flag = 0;
}