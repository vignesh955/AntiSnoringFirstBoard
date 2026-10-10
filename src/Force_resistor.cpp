#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SPI.h>
#include "Solenoid_valve.h"
#include "Force_resistor.h"

Adafruit_ADS1X15 adsSensorPad;

/**
 * @brief Initialize the force resistor sensor
 */

void forceInit()
{

  Wire.begin(4, 5);
  delay(1000);
  while (!adsSensorPad.begin(0x48))
  {
    Serial.println("Force sensor not detected");
    delay(1000);
  }
  delay(100);
}

/**
 * @brief Read Force resistor sensor
 * @param Sensor Number
 * @return The Force resistor reading
 */

float getAverageReading(int8_t sensor_number)
{

  float total = 0;

  for (int i = 0; i < 20; i++)
  {
    total += adsSensorPad.readADC_SingleEnded(sensor_number);
    delay(15);
  }
  return total / 20;
}

/**
 * @brief Convert ADC reading to grams
 * @param Raw ADC reading
 * @return The weight in grams
 */

float unitsToGrams(int units)
{
  // Calibration coefficients
  float m = -6.793;  // Slope
  float b = 3448.14; // Intercept

  // Convert units to grams
  float grams = m * units + b;
  return grams;
}

/**
 * @brief Find the Solenoid valve number to open in Phase-2
 * @return The Valve number
 */

int findValveNumber()
{

  int16_t adcValues[4];
  float weights[4];

  for (int i = 0; i < 4; i++)
  {

    adcValues[i] = getAverageReading(i);
    weights[i] = unitsToGrams(adcValues[i]);
  }

  int maxIndex = 0;
  for (int i = 0; i < 4; i++)
  {
    if (weights[i] >= weights[maxIndex])
    {
      maxIndex = i;
    }
  }

  return maxIndex + 1; // 4 - maxIndex
}

/**
 * @brief Read Force resistor sensor value and convert to grams
 * @param Sensor Number
 * @return The weight in grams
 */

float sensorReading(int8_t valveNumber)
{

  float adcReading = getAverageReading(valveNumber);
  float weight = unitsToGrams(adcReading);

  return weight;
}