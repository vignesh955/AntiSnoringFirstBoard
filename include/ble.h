#ifndef BLE_H
#define BLE_H
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ArduinoJson.h>

void bleSetup();
void sendBleMessage(const String &jsonString);
void sendBleStringMessage(const String &message);
#endif