/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
#include <stack>

//Device Configuration
static BLEAddress deviceAddr1 = BLEAddress("3c:71:bf:f9:f1:6a");
static BLEAddress deviceAddr2 = BLEAddress("cc:50:e3:a8:40:fe");
static BLEAddress deviceAddr3 = BLEAddress("a4:cf:12:1e:48:fa");
static BLEAddress deviceAddr4 = BLEAddress("3c:71:bf:71:00:36");

static BLERemoteCharacteristic* pGryoChar_1;

//Globals
static boolean doConnect = false;

//Scanning Related
static unsigned long ms;
static unsigned long lastEvent = 0;
static unsigned long threshold = 60000;
boolean scanning = true;

int TESTPIN = 4;

//Characteristic Map
byte REGNOTIF = 0x01;
byte MEGADATA = 0x02;
std::map<std::string,byte> charMap; //Current handles notification registration
std::stack <BLEAdvertisedDevice*> connectionWaitlist; 
std::stack <BLERemoteCharacteristic*> messageReadWaitlist; 

boolean newMail = false;

BLERemoteCharacteristic* newRemoteChar;

int numConnected = 0;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,uint8_t* pData,size_t length,bool isNotify) 
  {
    Serial.println("***********************");
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
    Serial.println("\n");
    messageReadWaitlist.push(pBLERemoteCharacteristic);
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
    lastEvent = millis(); //Start the search for the disconnected module
    Serial.print("Devices Connected: ");
    Serial.println(numConnected);
  }
};

bool connectToServer(BLEAdvertisedDevice* myDevice) {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println("Created client");
    pClient->setClientCallbacks(new MyClientCallback());
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    autoDiscover(pClient, true);
    Serial.println(" - Connected to server");
    
    Serial.println("Initialization Complete!");
    return(true);
}

void autoDiscover(BLEClient* pClient, bool subscribe)
{
  Serial.println("Avaliable Services:");
  std::map<std::string, byte>::iterator subMapItr; //Iterator for hashmap for auto register
  
  std::map<std::string, BLERemoteService*>* services = pClient->getServices();
  std::map<std::string, BLERemoteService*>::iterator itr;
  
  std::string selected_char; //Going to be the string of the currently select charecteristic
  for (itr = services->begin(); itr != services->end(); ++itr)
  {
    Serial.println((itr->first).c_str());
    std::map<std::string, BLERemoteCharacteristic*>* character = (itr->second)->getCharacteristics();
    std::map<std::string, BLERemoteCharacteristic*>::iterator charItr;
    for (charItr = character->begin(); charItr != character->end(); ++charItr)
    {
      selected_char = charItr->first; //Grab the selected charecteristic UUID in string form
      Serial.print("---");
      Serial.println((charItr->first).c_str());
      
      //Notification Registration if selected
      if(subscribe)
      {
        //subMapItr = charMap.find(selected_char);
        //if ((subMapItr != charMap.end()) && ((subMapItr->second)&REGNOTIF != 0))

        for (subMapItr = charMap.begin(); subMapItr != charMap.end(); ++subMapItr)
        {
            if((BLEUUID(subMapItr->first).equals(charItr->second->getUUID())) && ((subMapItr->second)&REGNOTIF != 0))
             {
              BLERemoteCharacteristic* selected_BLERemoteChar = charItr->second;
              selected_BLERemoteChar->registerForNotify(notifyCallback);
              Serial.println("--- Found in hashmap, registered for notifications!");
             }
        }
      }
    }
  }
}; 

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("* ");
    Serial.print(advertisedDevice.getAddress().toString().c_str());
    Serial.print("  ");
    Serial.println(advertisedDevice.toString().c_str());
   

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.getAddress().equals(deviceAddr1) || advertisedDevice.getAddress().equals(deviceAddr2) ||  advertisedDevice.getAddress().equals(deviceAddr3) ||  advertisedDevice.getAddress().equals(deviceAddr4)) //Make this not hard coded
    {
      Serial.printf("!! Match with %s \n", advertisedDevice.getAddress().toString().c_str());
      //BLEDevice::getScan()->stop();
      BLEAdvertisedDevice* newDevice = new BLEAdvertisedDevice(advertisedDevice);
      connectionWaitlist.push(newDevice);
      Serial.println("Pushing to stack!");
      scanning = true;
    } 
  } 
}; 

void initLatency()
{
  pinMode(TESTPIN, OUTPUT);
  digitalWrite(TESTPIN, LOW);
}

void setup() 
{  
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  initLatency();
  //User Charecteristic Registration Requirements
  //Eventually there will be a way for user to enter what they want charecteristics they want to register for
  //For now its hard coded. Deal with it.
  charMap.insert(std::pair<std::string,byte>("f9fd0004-71ae-42c4-bd19-9d5e37ebf0",REGNOTIF));
  charMap.insert(std::pair<std::string,byte>("f9fd0001-71ae-42c4-bd19-9d5e37ebf0",REGNOTIF));
  charMap.insert(std::pair<std::string,byte>("f9fd0006-71ae-42c4-bd19-9d5e37ebf0",REGNOTIF));
  charMap.insert(std::pair<std::string,byte>("f9fd0008-71ae-42c4-bd19-9d5e37ebf0",REGNOTIF));
  charMap.insert(std::pair<std::string,byte>("770294ed-f345-4f8b-bf3e-063b52d314ab",REGNOTIF|MEGADATA));
   
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} 

void debugLED()
{
  digitalWrite(TESTPIN, HIGH);
}

void printStringAsBytes(std::string value)
{
  Serial.println(value.c_str());
  int len = value.length();
  Serial.print("The len is: ");
  Serial.println(len);

  for(int i = 0; i < len; i++)
  {
    Serial.print((uint8_t)value[i]);
    if(i+1 != len)
    {
      Serial.print(",");
    }
  }
}

void checkInbox()
{
  if(messageReadWaitlist.size() != 0)
  {
    Serial.print("New Message in Inbox! Messages Unread: ");
    Serial.println(messageReadWaitlist.size());
    BLERemoteCharacteristic* newValue = messageReadWaitlist.top();

    
    std::string valueString = newValue->readValue();
    
    printStringAsBytes(valueString);
    
    messageReadWaitlist.pop();
    debugLED();
  }
}

void loop() {
  if (connectionWaitlist.size() != 0) {
    Serial.print("Remaining on Stack:");
    Serial.println(connectionWaitlist.size());
    connectToServer(connectionWaitlist.top());
    connectionWaitlist.pop();
  }
  
  ms = millis();
  if ((scanning) && (ms >= lastEvent + threshold)) {
    BLEDevice::getScan()->stop();
    scanning = false;
    Serial.println("Scanning Complete!");
    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!");
  }

  Serial.print("numConnected: ");
  Serial.println(numConnected);
  if (numConnected > 0) {
    Serial.println("There is a device connected");
    checkInbox();
    
  }
  else
  {
    Serial.println("0 are connected right now!");
    numConnected = 0;
  }

  
  delay(1000); // Delay a second between loops.
}
