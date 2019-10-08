/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

// Setting up Services/Chars
static BLEUUID GYRO_SERVICE_UUID("f9fd0000-71ae-42c4-bd19-9d5e37ebf073");
static        BLEUUID GYRO_CHAR_1("f9fd0001-71ae-42c4-bd19-9d5e37ebf073");


static BLEUUID LIGHT_SERVICE_UUID("605b0000-0d8f-4002-abb3-3eb9a9c388ea");
static        BLEUUID    LIGHT_CHAR_1("f9fd0001-71ae-42c4-bd19-9d5e37ebf073");

static BLEUUID TEMP_SERVICE_UUID("b58d0000-066d-4f45-99fa-60c588735e5d");
static        BLEUUID    TEMP_CHAR_1("b58d0001-066d-4f45-99fa-60c588735e5d");



static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

static BLERemoteCharacteristic* pGryoChar_1;
static BLERemoteCharacteristic* pLightChar_1;
static BLERemoteCharacteristic* pTempChar_1;

static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.println("connectToServer()");
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    
    // Connect to Services
    BLERemoteService* pGyroService = pClient->getService(GYRO_SERVICE_UUID);
    BLERemoteService* pLightService = pClient->getService(LIGHT_SERVICE_UUID);
    BLERemoteService* pTempService = pClient->getService(TEMP_SERVICE_UUID);
    Serial.println("Services are up!");
    if (pGyroService == nullptr) {
      Serial.print("Failed to find Service: Gyro");
      pClient->disconnect();
      return false;
    }
    if (pLightService == nullptr) {
      Serial.print("Failed to find Service: Light");
      pClient->disconnect();
      return false;
    }
    if (pTempService == nullptr) {
      Serial.print("Failed to find Service: Temp");
      pClient->disconnect();
      return false;
    }

    pGryoChar_1 = pGyroService->getCharacteristic(GYRO_CHAR_1);
    pLightChar_1 = pLightService->getCharacteristic(LIGHT_CHAR_1);
    pTempChar_1 = pTempService->getCharacteristic(TEMP_CHAR_1);
    
    if (pGryoChar_1 == nullptr) {
      Serial.print("Failed to find Char: Gyro ");
      pClient->disconnect();
      return false;
    }
    if (pLightChar_1 == nullptr) {
      Serial.print("Failed to find Char: Light");
      pClient->disconnect();
      return false;
    }
    if (pTempChar_1 == nullptr) {
      Serial.print("Failed to find Char: Temp");
      pClient->disconnect();
      return false;
    }
    
    Serial.println("Found our services");
    
    connected = true;
}
/**
 * Scan for BLE servers and find the first one that adverti
 * ses the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(GYRO_SERVICE_UUID)) {
      Serial.println("Have a match!");
      BLEDevice::getScan()->stop();
      Serial.println("myDevice pointer made");
      myDevice = new BLEAdvertisedDevice(advertisedDevice);

      Serial.println("We are out!");
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String gValue = "gValue";
    String lValue = "lValue";
    String tValue = "tValue";

    Serial.println("We are about to write");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pGryoChar_1->writeValue(gValue.c_str(), gValue.length());
    Serial.println("Wrote new characteristic (Gyro) \"" + gValue + "\"");
    pLightChar_1->writeValue(lValue.c_str(), lValue.length());
    Serial.println("Wrote new characteristic (Light) \"" + lValue + "\"");
    pTempChar_1->writeValue(tValue.c_str(), tValue.length());
    Serial.println("Setting new characteristic (Temp) \"" + tValue + "\"");
    
  }
  
  else if(doScan)
  {
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); // Delay a second between loops.
}
