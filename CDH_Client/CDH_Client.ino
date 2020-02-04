//CDH BLE Module For Rowan University Wireless Cubesat Clinic Project
//2019-2020: Tim Roche, Kenny Wagner, Jake Matteo

#include "BLEDevice.h"
#include <stack>
#include "charStruct.h"
#include "parser.h" //For parsing functions, its really small right now but it should grow in the future
#include <string>

//UART COMMS
#define RXD2 16
#define TXD2 17

//Device Configuration
static BLEAddress deviceAddr1 = BLEAddress("3c:71:bf:f9:f1:6a");
static BLEAddress deviceAddr2 = BLEAddress("cc:50:e3:a8:40:fe");
static BLEAddress deviceAddr3 = BLEAddress("a4:cf:12:1e:48:fa"); 
static BLEAddress deviceAddr4 = BLEAddress("3c:71:bf:71:00:36");

//Globals
static boolean doConnect = false;

//Scanning Related
static unsigned long ms;
static unsigned long lastEvent = 0;
static unsigned long threshold = 60000;
boolean scanning = true;

//Characteristic Map
byte REGNOTIF = 0x01;
byte MEGADATA = 0x02;
std::map<std::string, charStruct> charMap; //Current handles notification registration
std::stack <BLEAdvertisedDevice*> connectionWaitlist; 
std::stack < std::pair<BLERemoteCharacteristic*,uint8_t > > messageReadWaitlist;

boolean photoTran = false;

int numConnected = 0;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,uint8_t* pData,size_t length,bool isNotify) 
  {
    Serial.println("***********************");
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.println(*pData);
    Serial.print(" of data length ");
    Serial.println(length);
    messageReadWaitlist.push( std::pair<BLERemoteCharacteristic*, uint8_t>(pBLERemoteCharacteristic, *pData));
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



void autoDiscover(BLEClient* pClient, bool subscribe)
{
  Serial.println("Avaliable Services:");
  std::map<std::string, charStruct>::iterator subMapItr; //Iterator for hashmap for auto register
 
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
      //selected_char = charItr->first; //Grab the selected charecteristic UUID in string form
      Serial.print("---");
      Serial.println((charItr->first).c_str());


      //Sets BLERemoteCharecteristics and enables notifications if set when function was called
        for (subMapItr = charMap.begin(); subMapItr != charMap.end(); ++subMapItr)
        {
            if((BLEUUID(subMapItr->first).equals(charItr->second->getUUID()))) 
            {
                BLERemoteCharacteristic* selected_BLERemoteChar = charItr->second;
                subMapItr->second.setCharecteristic(selected_BLERemoteChar);
                if((subscribe) && ((subMapItr->second.getSettings())& REGNOTIF != 0))
                 {
                    selected_BLERemoteChar->registerForNotify(notifyCallback);
                    Serial.println("--- Found in hashmap, registered for notifications!");
                 }
            }

        }
      }
    }
  } 

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


void interpretCommand(std::string input)
{
  std::string modifier = getValue(input, ' ', 0);
  std::string  UUID    = getValue(input, ' ', 1); // "770294ed-f345-4f8b-bf3e-063b52d314ab";
  std::string  payload   = getValue(input, ' ' , 2, true); //guard = true, allows for spaces in data
  Serial.print("Modifier: ");
  Serial.println(modifier.c_str());
  Serial.print("UUID: ");
  Serial.println(UUID.c_str());
  Serial.print("Payload: ");
  Serial.println(payload.c_str());

  /*if(modifier == "Show")
  {

  }
  if(modifier == "Connect")
  {
    //Insert into Charmap
    //Autodiscover
  }*/
  std::map<std::string, charStruct>::iterator it;
  it = charMap.find(UUID);
  if (it == charMap.end())
  {
    Serial.println("Cannot find in char map!");
    return;
  }
  charStruct out = it->second;
  BLERemoteCharacteristic* bleChar = out.getCharecteristic();
  if (modifier == "Update")
  {
    //Impliment Large Data support
     bleChar->writeValue(payload); 
     Serial.println("Update Complete!");
     Serial2.println("[Completed]");
  }
  if (modifier == "Read")
  {
      int size = 0; //This is temporary until I impliment large data support again
      readCharecteristic(bleChar, size);
      Serial.println("Read Complete!");
  }

}

//Initilizes UART between PC -> ESP
//and between ESP -> MCU
void init_UART()
{
  Serial.begin(115200);   
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
}

void setup() 
{  
  init_UART();
  Serial.println("CDH");
  //User Charecteristic Registration Requirements
  //Eventually there will be a way for user to enter what they want charecteristics they want to register for
  //For now its hard coded. Deal with it.
  charMap.insert(std::pair<std::string,charStruct>("f9fd0006-71ae-42c4-bd19-9d5e37ebf073",charStruct(REGNOTIF)));
  charMap.insert(std::pair<std::string,charStruct>("f9fd0016-71ae-42c4-bd19-9d5e37ebf073",charStruct(REGNOTIF)));
  charMap.insert(std::pair<std::string,charStruct>("f9fd0017-71ae-42c4-bd19-9d5e37ebf073",charStruct(REGNOTIF)));

  charMap.insert(std::pair<std::string,charStruct>("770294ed-f345-4f8b-bf3e-063b52d314ab",charStruct(REGNOTIF)));
  
  BLEDevice::init("CDH");
  BLEDevice::setPower(ESP_PWR_LVL_N14);
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} 



void printStringAsBytes(std::string value, bool verb)
{
  //Serial.println(value.c_str());
  int len = value.length();
  if(verb)
  {
    Serial.print("The len is: ");
    Serial.println(len);
  }

  for(int i = 0; i < len; i++)
  {
    Serial.print((uint8_t)value[i]);
    if(i+1 != len)
    {
      Serial.print(",");
    }
  }
  Serial.println();
}

charStruct* searchCharMap(BLERemoteCharacteristic* newValue)
{
  std::map<std::string, charStruct>::iterator subMapItr; //Iterator for hashmap for auto register
  for (subMapItr = charMap.begin(); subMapItr != charMap.end(); ++subMapItr)
  {
      if(BLEUUID(subMapItr->first).equals(newValue->getUUID()))
       {
          return(&(subMapItr->second));
       }
  }
  return(NULL);
}


//Eventually will return something, for now just prints out the value
void readCharecteristic(BLERemoteCharacteristic* newValue, uint8_t size)
{
  Serial.println("In readCharecteristics");
  std::string valueString;
  //BLERemoteCharacteristic* newValue = valuePair.first;
  //uint8_t message = valuePair.second;
  byte flags = searchCharMap(newValue) -> getSettings();
  Serial.println(size);
  Serial.println(flags);
  if((size != 0) && (flags&MEGADATA != 0))
  {
    photoTran = true;
    int tranNumber = (int)(size);
    Serial.print("Large Data! Message Len: ");
    Serial.println(tranNumber);
    int iterator = 0;
    int EOT = false; //end of transmission sent by server
    while((EOT == false) && (iterator < tranNumber))
    {
      valueString = newValue->readValue();
      if(valueString == "")
      {
        Serial.println("I recieved an End of Transmission before I thought I would!");
        EOT = true;
      }
      else
      {
        printStringAsBytes(valueString, false);
      }
      iterator++;
    }
    photoTran = false;
  }
  else
  {
    Serial.println("Normal Transmission");
    valueString = newValue->readValue();
    //printStringAsBytes(valueString, false);
    Serial.println(valueString.c_str());
    Serial2.println(valueString.c_str());
  }
}

void checkForCommands()
{
  int incomingByte = 0; // incoming byte from serial input
  char c;
  String output = "";

  while (Serial.available() > 0) 
  {
    incomingByte = Serial.read();
    c = (char) incomingByte;
    Serial.print(c);
    output += c; 
  }
  if(output != "")
  {
    interpretCommand(output.c_str());
  }
}

void checkInbox()
{
  if(messageReadWaitlist.size() != 0)
  {
    
    Serial.print("New Message in Inbox! Messages Unread: ");
    Serial.println(messageReadWaitlist.size());
    std::pair<BLERemoteCharacteristic*,uint8_t> notifyPair = messageReadWaitlist.top();
    int t = millis();
    BLERemoteCharacteristic* newValue = notifyPair.first;
    uint8_t size = notifyPair.second;
    readCharecteristic(newValue, size);
    Serial.print("Read Time: ");
    Serial.println(millis() - t);
    messageReadWaitlist.pop();
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

  if (numConnected > 0) {
    //Serial.println("There is a device connected");
    checkInbox();
  }
  else
  {
    Serial.println("0 are connected right now!");
    numConnected = 0;
  }

  checkForCommands();
  delay(120);
}
