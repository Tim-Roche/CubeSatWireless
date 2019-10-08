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

#define  GYRO_SERVICE_UUID  "f9fd0000-71ae-42c4-bd19-9d5e37ebf073"
#define  GYRO_CHAR_1        "f9fd0001-71ae-42c4-bd19-9d5e37ebf073"

#define LIGHT_SERVICE_UUID  "605b0000-0d8f-4002-abb3-3eb9a9c388ea"
#define  LIGHT_CHAR_1       "605b0001-0d8f-4002-abb3-3eb9a9c388ea"

#define  TEMP_SERVICE_UUID  "b58d0000-066d-4f45-99fa-60c588735e5d"
#define  TEMP_CHAR_1        "b58d0001-066d-4f45-99fa-60c588735e5d"

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
  pGryoChar_1 = pGyroService->createCharacteristic(                                // Create the characteristic UUID for server
                                         GYRO_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pGryoChar_1->setCallbacks(new MyCallbacks()); 

  pLightChar_1 = pLightService->createCharacteristic(                                // Create the characteristic UUID for server
                                         LIGHT_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pLightChar_1->setCallbacks(new MyCallbacks()); 

  pTempChar_1 = pTempService->createCharacteristic(                                // Create the characteristic UUID for server
                                         TEMP_CHAR_1,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pTempChar_1->setCallbacks(new MyCallbacks()); 

  pGryoChar_1->setValue("100"); 
  pTempChar_1->setValue("250"); 

  pGyroService->start();                                                            // Start service
  pLightService->start();                                                            // Start service
  pTempService->start();                                                            // Start service
                         
  pServer->setCallbacks(new MyServerCallbacks());                               // Set server callbacks
  

  //Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   // Grab advertisiing service
  
  pAdvertising->addServiceUUID(GYRO_SERVICE_UUID);                                   // Add service to advertising
  pAdvertising->addServiceUUID(LIGHT_SERVICE_UUID);   
  pAdvertising->addServiceUUID(TEMP_SERVICE_UUID);   
  
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();                                                // Start advertising
  Serial.println("Setup Complete. We are Advertising!");


}

bool isTest = true;
void loop() {
  if (deviceConnected) {
 /*   //SENDING
      if(isTest)
        {  
        pGryoChar_1->setValue("100"); 
        Serial.println("pGryoChar_1 set to: 100"); 
        delay(10);

        pTempChar_1->setValue("-200");
        Serial.println("pTempChar_1 set to: -200"); 
        
        isTest=false;
        }
      else
      {
        pGryoChar_1->setValue("5000"); 
        Serial.println("pGryoChar_1 set to: 5000"); 
        delay(10);

        pTempChar_1->setValue("2500");
        Serial.println("pTempChar_1 set to: 2500"); 
        
        isTest=false;
      }*/
      pGryoChar_1->notify();
      pTempChar_1->notify();
      delay(100);
  }
  else
  {
     Serial.println("Device is not Connected");
     delay(100);
  }
  
}
