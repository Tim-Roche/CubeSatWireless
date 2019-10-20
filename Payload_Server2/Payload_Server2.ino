/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#include "esp32-hal-log.h"


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define  TEST_SERVICE_UUID  "f9fd0000-71ae-42c4-bd19-9d5e37ebf073"
#define  TEST_CHAR_1        "f9fd0001-71ae-42c4-bd19-9d5e37ebf073"

BLEServer *pServer = NULL;
BLECharacteristic * pTESTChar_1;

bool deviceConnected = false;
/********** Server Callback to restart advertising after a subsystem conencts **********/
class MyServerCallbacks: public BLEServerCallbacks {
  // TODO this doesn't take into account several clients being connected
    
    void onConnect(BLEServer* pServer) {
      BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
      pAdvertising->start();
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string uuid = pCharacteristic->getUUID().toString();
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("From UUID: ");
        Serial.println(uuid.c_str());
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }

    void onRead(BLECharacteristic* pCharacteristic) {
      String uuid = pCharacteristic->getUUID().toString().c_str();
      Serial.print("Someone wants to read my data: ");
      Serial.println(uuid.c_str());
      if(uuid == TEST_CHAR_1)
      {
         String message = "Test!"+String(t);
         Serial.print("Transmitting: ");
         Serial.println(message.c_str());
         pCharacteristic->setValue(message.c_str());
      }
      else
      {
        Serial.println("Unknown UUID!");  
      }
   }
};

void setup()
{
  Serial.begin(115200);                                                         // Initialize serial port
  Serial.println("Starting BLE work!");

  BLEDevice::init("C&DH");                                                      // Initialize the BLE device
  BLEServer *pServer = BLEDevice::createServer();                               // Save the BLE device server

  //Creating Services
  BLEService *pGyroService = pServer->createService(TEST_SERVICE_UUID);                 // Create the service UUID from the server
  
  //Creating Characteristics
  pGryoChar_1 = pGyroService->createCharacteristic(                                // Create the characteristic UUID for server
                                         TEST_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ  |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  
  pGryoChar_1->setCallbacks(new MyCallbacks()); 

  pTestChar_1->addDescriptor(new BLE2902());

  pTestService->start();                                                            // Start service
                         
  pServer->setCallbacks(new MyServerCallbacks());                               // Set server callbacks
  
  //Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   // Grab advertisiing service
  
  pAdvertising->addServiceUUID(TEST_SERVICE_UUID);   
  
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();                                                // Start advertising
  Serial.println("Setup Complete. We are Advertising!");


}

void loop() {
  if (deviceConnected) {
    Serial.println("Payload 2 Connected!");
    t++;
    if(t >= 10)
    {
      Serial.println("Notifying Test");
      pTestChar_1->notify();
      t = 0;
    }
    delay(1000);
  }
  else
  {
     Serial.println("Device is not Connected");
     delay(100);
  }
  
}
