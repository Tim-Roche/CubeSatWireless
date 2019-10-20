/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"

//Device Configuration
static BLEAddress deviceAddr1 = BLEAddress("3c:71:bf:f9:f1:6a");
static BLEAddress deviceAddr2 = BLEAddress("FF:FF:3D:1A:C2:66");

static BLERemoteCharacteristic* pGryoChar_1;
static BLEAdvertisedDevice* myDevice;

//Globals
static boolean doConnect = false;

//Scanning Related
static unsigned long ms;
static unsigned long lastEvent = 0;
static unsigned long threshold = 10000;
boolean scanning = true;

boolean newMail = false;

BLERemoteCharacteristic* newRemoteChar;

int numConnected = 0;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.println("***********************");
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
    Serial.println("\n");
    newRemoteChar = pBLERemoteCharacteristic;
    newMail = true;
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
        Serial.printf("device connected: %s\n", pclient->getPeerAddress().toString().c_str());
        numConnected++;
        Serial.print("Devices Connected: ");
        Serial.println(numConnected);
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.printf("device disconnected: %s\n", pclient->getPeerAddress().toString().c_str());
    Serial.println("onDisconnect");
    numConnected--;
    Serial.print("Devices Connected: ");
    Serial.println(numConnected);
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println("Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    
    Serial.println("Initialization Complete!");
    return(true);
}


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("* ");
    Serial.print(advertisedDevice.getAddress().toString().c_str());
    Serial.print("  ");
    Serial.println(advertisedDevice.toString().c_str());
   

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.getAddress().equals(deviceAddr1) || advertisedDevice.getAddress().equals(deviceAddr2))
    {
      Serial.printf("!! Match with %s \n", advertisedDevice.getAddress().toString().c_str());
      //BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      scanning = true;
    } 
  } 
}; 

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} 


void loop() {
  if (doConnect == true) {
    connectToServer();
    doConnect = false;
  }
  
  ms = millis();
  if ((scanning) && (ms >= lastEvent + threshold)) {
    BLEDevice::getScan()->stop();
    scanning = false;
    Serial.println("Scanning Complete!");
  }
  
  if (numConnected != 0) {
    Serial.println("There is a device connected");
  }

  
  delay(1000); // Delay a second between loops.
}
