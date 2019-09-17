/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

int LED_B = 2;

/********** Server Callback to restart advertising after a subsystem conencts **********/
class MyServerCallbacks: public BLEServerCallbacks {
  // TODO this doesn't take into account several clients being connected
    void onConnect(BLEServer* pServer) {
      BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
      pAdvertising->start();
    };
    void onDisconnect(BLEServer* pServer) {
    }
};


void setup() {
  pinMode (LED_B, OUTPUT);                                                // Set Builtin LED to output
  
  Serial.begin(115200);                                                         // Initialize serial port
  Serial.println("Starting BLE work!");

  BLEDevice::init("C&DH");                                                      // Initialize the BLE device
  BLEServer *pServer = BLEDevice::createServer();                               // Save the BLE device server
  BLEService *pService = pServer->createService(SERVICE_UUID);                  // Create the service UUID from the server
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(          // Create the characteristic UUID for server
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

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

void loop() {
  digitalWrite(LED_B, HIGH);
  delay(100);
  digitalWrite(LED_B, LOW);
  delay(100);
//  put your main code here, to run repeatedly:
//  delay(2000);
}
