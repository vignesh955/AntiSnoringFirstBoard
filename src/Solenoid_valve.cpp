#include <Arduino.h>
#include "Solenoid_valve.h"
#include "state.h"
#include "watchDog.h"

/**
 * @brief Initialize the solenoid valves
 */

void solenoidValveInit()
{

  pinMode(VALVE_1, OUTPUT);
  pinMode(VALVE_2, OUTPUT);
  pinMode(VALVE_3, OUTPUT);
  pinMode(VALVE_4, OUTPUT);
  pinMode(VALVE_5, OUTPUT);
  pinMode(DEFLATION, OUTPUT);
  delay(300);
  delay(100);
  turnOffAllValves();
  delay(100);
}

/**
 * @brief Turn on the Solenoid valves
 */

void turnOnAllValves()
{

  turnOffAllValves();
  delay(100);

  digitalWrite(DEFLATION, HIGH);
  delay(10);

  digitalWrite(VALVE_1, HIGH);
  delay(10);

  digitalWrite(VALVE_2, HIGH);
  delay(10);

  digitalWrite(VALVE_3, HIGH);
  delay(10);

  uint32_t timeOut = millis(); // check

  uint32_t lastWatchdogReset = millis();

  while (millis() - timeOut < 5000) // wait for 5 seconds
  {
    if (currentState != STATE_DEFAULT)
    {
      break;
    }

    if (millis() - lastWatchdogReset > 2000)
    {
      feedWatchdog();
      lastWatchdogReset = millis();
    }
  }
  digitalWrite(VALVE_1, LOW);
  delay(10);

  digitalWrite(VALVE_2, LOW);
  delay(10);

  digitalWrite(VALVE_3, LOW);
  delay(10);

  digitalWrite(VALVE_4, HIGH);
  delay(10);

  digitalWrite(VALVE_5, HIGH);
  delay(10);

  timeOut = millis(); // check
  lastWatchdogReset = millis();

  while (millis() - timeOut < 5000) // wait for 5 seconds
  {

    if (currentState != STATE_DEFAULT)
    {
      break;
    }

    if (millis() - lastWatchdogReset > 2000)
    {
      feedWatchdog();
      lastWatchdogReset = millis();
    }
  }

  digitalWrite(VALVE_4, LOW);
  delay(10);

  digitalWrite(VALVE_5, LOW);
  delay(10);

  digitalWrite(DEFLATION, LOW);
  delay(10);
}

/**
 * @brief Turn off all the valves
 */

void turnOffAllValves()
{
  digitalWrite(VALVE_1, LOW);
  delay(50);

  digitalWrite(VALVE_2, LOW);
  delay(50);

  digitalWrite(VALVE_3, LOW);
  delay(50);

  digitalWrite(VALVE_4, LOW);
  delay(50);

  digitalWrite(VALVE_5, LOW);
  delay(50);

  digitalWrite(DEFLATION, LOW);
  delay(50);
}

/**
 * @brief Turn on the specific Solenoid valve
 * @param Valve Number
 */

void solenoidValveOn(int valveNumber)
{
  switch (valveNumber)
  {
  case 1:
    turnOffAllValves();
    delay(100);
    digitalWrite(VALVE_1, HIGH);
    Serial.println("Valve 1 is ON");
    delay(100);

    break;
  case 2:
    turnOffAllValves();
    delay(100);
    digitalWrite(VALVE_2, HIGH);
    Serial.println("Valve 2 is ON");
    delay(100);

    break;
  case 3:
    turnOffAllValves();
    delay(100);
    digitalWrite(VALVE_3, HIGH);
    Serial.println("Valve 3 is ON");
    delay(100);

    break;
  case 4:
    turnOffAllValves();
    delay(100);
    digitalWrite(VALVE_4, HIGH);
    Serial.println("Valve 4 is ON");
    delay(100);

    break;
  case 5:
    turnOffAllValves(); // in default state all valves are open, first turn off
    delay(100);
    digitalWrite(VALVE_5, HIGH);
    Serial.println(" Master airbag is ON");
    delay(100);
    break;
  case 6:
    digitalWrite(DEFLATION, HIGH);
    delay(100);
    break;
  default:
    Serial.println("Invalid valve number");
    break;
  }
}

/**
 * @brief Turn off specific valve
 * @param Valve Number
 */

void solenoidValveOff(int valveNumber)
{
  switch (valveNumber)
  {
  case 1:
    digitalWrite(VALVE_1, LOW);
    Serial.println("Valve 1 is Off");
    break;
  case 2:
    digitalWrite(VALVE_2, LOW);
    Serial.println("Valve 2 is Off");
    break;
  case 3:
    digitalWrite(VALVE_3, LOW);
    Serial.println("Valve 3 is Off");
    break;
  case 4:
    digitalWrite(VALVE_4, LOW);
    Serial.println("Valve 4 is Off");
    break;
  case 5:
    digitalWrite(VALVE_5, LOW);
    Serial.println(" Master airbag is Off");
    break;
  case 6:
    digitalWrite(DEFLATION, LOW);
    break;
  default:
    Serial.println("Invalid valve number");
    break;
  }
}
