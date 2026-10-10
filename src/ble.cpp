#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "state.h"
#include "flag.h"
#include "ble.h"

// State currentState;
int flag = 0;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer *pServer = nullptr;
bool deviceConnected = false;                 // Track connection status
BLECharacteristic *pCharacteristic = nullptr; // Global variable
bool isAuthenticated = false;                 // Track authentication status

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onRead(BLECharacteristic *pCharacteristic) override
  {
    Serial.println("Characteristic read");

    pCharacteristic->setValue("Hello World says Neil");
  }

  void onWrite(BLECharacteristic *pCharacteristic) override
  {

    if (flag == 0)
    {
      // Get the value written by the client
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0)
      {
        Serial.print("Received data from client: ");
        for (size_t i = 0; i < value.length(); i++)
        {
          Serial.print(value[i]);
        }
        Serial.println();
      }

      if (value.length() > 0)
      {
        

        // Update the state based on the received value
        if (value == "PHASE1")
        {
          currentState = STATE_PHASE1;
          Serial.println("State updated to: PHASE1");
        }
        else if (value == "PHASE2")
        {
          currentState = STATE_PHASE2;
          
        }
        else if (value == "PHASE3")
        {
          currentState = STATE_PHASE3;
          
        }
        else
        {
          currentState = STATE_DEFAULT;
          
        }
      }
    }
  }
};

// Server Callbacks to detect connection and disconnection
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Client connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    isAuthenticated = false;

    Serial.println("Client disconnected");
    pServer->startAdvertising(); // Restart advertising for new connections
  }
};

// Security Callbacks for authentication events
class MySecurity : public BLESecurityCallbacks
{
  bool onConfirmPIN(uint32_t pin)
  {
    Serial.print("Confirming PIN: ");
    Serial.println(pin);
    if (pin >= 100000 && pin <= 999999) // Check if it's a 6-digit number
    {
      return true; // Accept the PIN
    }
    else
    {
      Serial.println("Invalid PIN length. Disconnecting...");
      pServer->disconnect(0); // Disconnect if PIN is invalid
      return false;           // Reject the PIN
    }
  }

  uint32_t onPassKeyRequest()
  {
    Serial.println("Passkey requested");

    return 530841; // Provide static passkey
  }

  void onPassKeyNotify(uint32_t pass_key)
  {
    Serial.print("Passkey notify: ");
    Serial.println(pass_key);
  }

  bool onSecurityRequest()
  {
    Serial.println("Security request received");
    return true; // Accept the security request
  }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
  {
    if (cmpl.success)
    {
      Serial.println("Authentication successful!");
      isAuthenticated = true;
    }
    else
    {
      Serial.println("Authentication failed. Disconnecting...");
      isAuthenticated = false;
      pServer->disconnect(0); // Disconnect if authentication fails
    }
  }
};

/**
 * @brief Bluetooth setup
 */

void bleSetup()
{
  delay(3000); // Allow time to open Serial Monitor
  Serial.begin(115200);
  Serial.println("Starting BLE...");

  BLEDevice::init("QYTNITE-QN500");
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  BLEDevice::setSecurityCallbacks(new MySecurity());

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  pCharacteristic->addDescriptor(new BLE2902());

  pCharacteristic->setValue("Hello World says Neil");
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // For sending large data (Notify)
  BLEDevice::setMTU(256);

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setStaticPIN(530841);
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
  pSecurity->setCapability(ESP_IO_CAP_OUT); // Output-only capability

  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  // clearBondedDevices();

  Serial.println("BLE setup complete. Waiting for connections...");
}

/**
 * @brief Notify the BLE client
 * @param JSON Message
 */

void sendBleMessage(const String &jsonString)
{
  

  if (pCharacteristic != nullptr && isAuthenticated)
  {
    Serial.print("Sending JSON over BLE: ");
    Serial.println(jsonString);

    // Send JSON as BLE notification
    pCharacteristic->setValue(jsonString.c_str());
    pCharacteristic->notify();
  }
  delay(100);
}

void sendBleStringMessage(const String &message)
{
  if (pCharacteristic != nullptr && isAuthenticated)
  {
    Serial.print("Sending String over BLE: ");
    Serial.println(message);

    // Send the string as a BLE notification
    pCharacteristic->setValue(message.c_str());
    pCharacteristic->notify();
  }
  delay(100);  // Small delay to prevent flooding BLE notifications
}
