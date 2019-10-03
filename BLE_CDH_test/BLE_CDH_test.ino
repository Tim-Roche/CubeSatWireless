/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define  GYRO_SERVICE_UUID = "f9fd0000-71ae-42c4-bd19-9d5e37ebf073"
#define  GYRO_CHAR_1       = "f9fd0001-71ae-42c4-bd19-9d5e37ebf073"

#define LIGHT_SERVICE_UUID = "605b0000-0d8f-4002-abb3-3eb9a9c388ea"
#define  LIGHT_CHAR_1      = "f9fd0001-71ae-42c4-bd19-9d5e37ebf073"

#define  TEMP_SERVICE_UUID = "b58d0000-066d-4f45-99fa-60c588735e5d"
#define  TEMP_CHAR_1       = "b58d0001-066d-4f45-99fa-60c588735e5d"

BLEServer *pServer = NULL;
BLECharacteristic * pGryoChar_1;
BLECharacteristic * pLightChar_1;
BLECharacteristic * pTempChar_1;



int LED_B = 2;
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
      std::string value = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

void setup()
{
  pinMode (LED_B, OUTPUT);                                                // Set Builtin LED to output
  
  Serial.begin(115200);                                                         // Initialize serial port
  Serial.println("Starting BLE work!");

  BLEDevice::init("C&DH");                                                      // Initialize the BLE device
  BLEServer *pServer = BLEDevice::createServer();                               // Save the BLE device server

  //Creating Services
  BLEService *pGyroService = pServer->createService(GYRO_SERVICE_UUID);                 // Create the service UUID from the server
  BLEService *pLightService = pServer->createService(LIGHT_SERVICE_UUID);                 // Create the service UUID from the server
  BLEService *pTempService = pServer->createService(TEMP_SERVICE_UUID);                 // Create the service UUID from the server

  //Creating Characteristics
  pGryoChar_1 = pService->createCharacteristic(                                // Create the characteristic UUID for server
                                         GYRO_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristicrx->setCallbacks(new MyCallbacks()); 

  pLightChar_1 = pService->createCharacteristic(                                // Create the characteristic UUID for server
                                         LIGHT_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristicrx->setCallbacks(new MyCallbacks()); 

  pTempChar_1 = pService->createCharacteristic(                                // Create the characteristic UUID for server
                                         TEMP_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristicrx->setCallbacks(new MyCallbacks()); 
                         
  pServer->setCallbacks(new MyServerCallbacks());                               // Set server callbacks
  pService->start();                                                            // Start service

  //Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   // Grab advertisiing service
  
  pAdvertising->addServiceUUID(SERVICE_UUID);                                   // Add service to advertising
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();                                                // Start advertising
  Serial.println("Setup Complete. We are Advertising!");
}

bool isTest = true;
void loop() {
  if (deviceConnected) {
    //SENDING
      if(isTest)
        {  
        pCharacteristic->setValue("Test");  
        Serial.println("Sent: Test!");
        isTest=false;
        }
      else
      {
        pCharacteristic->setValue("Something Else");  
        Serial.println("Sent: Something Else!");
        isTest=true;
      }
      pCharacteristic->notify();
      delay(100);
  }
  else
  {
     Serial.println("Device is not Connected");
     delay(100);
  }
  
}
