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

#define SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID2 "cb673139-b730-48b0-a7ba-c04458f5328e"

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

BLEServer *pServer = NULL;
BLECharacteristic * pCharacteristic;
BLECharacteristic * pCharacteristicrx;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

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
  BLEService *pService = pServer->createService(SERVICE_UUID);                  // Create the service UUID from the server
  pCharacteristic = pService->createCharacteristic(          // Create the characteristic UUID for server
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristicrx = pService->createCharacteristic(          // Create the characteristic UUID for server
                                         CHARACTERISTIC_UUID2,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristicrx->setCallbacks(new MyCallbacks());                          
  pServer->setCallbacks(new MyServerCallbacks());                               // Set server callbacks
  pCharacteristic->setValue("PayloadLedOff");                                   // Characteristic initial value
  pService->start();                                                            // Start service
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   // Grab advertisiing service
  pAdvertising->addServiceUUID(SERVICE_UUID);                                   // Add service to advertising
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();                                                // Start advertising
  Serial.println("Characteristic defined! Now you can read it in your phone!");
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
     Serial.println("Device is not Connected :(");
  }
  
}
