#include <Arduino.h>
#include "Air_pump.h"
#include "Solenoid_valve.h"
#include "ble.h"
#include "state.h"
#include "actionPhase.h"
#include "Force_resistor.h"
#include "Piezo_senosr.h"
#include "flag.h"
#include "watchDog.h"
#include "initAirbag.h"

State currentState = STATE_DEFAULT;

void setup()
{

  delay(3000);

  Serial.begin(115200);
  delay(1000);

  bleSetup();
  Serial.println("BLE Initialization completed");

  airPumpInit();
  Serial.println("Airpump Initialization completed");

  solenoidValveInit();
  Serial.println("Solenoid Valve Initialization completed");
  delay(1000);

  forceInit();
  Serial.println("Force Resistor Sensor Initialization completed");

  piezoInit();
  Serial.println("piezo Sensor Initialization completed");

  initWatchdog(10);
  initAirbagPressure(); // initial airbag pressure value

  delay(500);
}

void loop()
{

  feedWatchdog();

  switch (currentState)
  {
  case STATE_DEFAULT:
    actionDefault();
    break;
  case STATE_PHASE1:
    actionPhase1();
    break;
  case STATE_PHASE2:
    actionPhase2();
    break;
  case STATE_PHASE3:
    actionPhase3();
    break;
  }

  delay(100);
}
