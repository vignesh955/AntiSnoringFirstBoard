#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>

#include "Piezo_senosr.h"

Adafruit_ADS1015 adsPeizoSensor;

/**
 * @brief Piezo sensor initialization
 */

void piezoInit()
{

  Wire.begin(4, 5); // alreadry begun in force_init()
  delay(1000);
  adsPeizoSensor.setGain(GAIN_EIGHT);
  while (!adsPeizoSensor.begin(0x49))
  {
    Serial.println("Piezo sensor not detecetd");
    delay(1000);
  } // expected address is 0x49
  delay(100);
}

/**
 * @brief Read the piezo sensor pressure reading
 * @return The pressure reading
 */

float piezoSensorReading()
{

  float results;

  float sum = 0;
  const int numReadings = 20;

  // Take 20 readings and compute the average
  for (int i = 0; i < numReadings; i++)
  {
    sum += adsPeizoSensor.readADC_Differential_0_1();
    delay(10); // Small delay to stabilize readings
  }

  results = sum / numReadings; // Average of 20 readings
  // delay(200);
  float pressureValue = results + 1;

  Serial.print("Pressure :");
  Serial.println(pressureValue); 
  return pressureValue;
}
