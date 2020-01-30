 /*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
//#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>
#include "picturebitmap.h"

#define TRANSMISSION_SIZE 500

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

std::string   TEST_SERVICE_UUID =  "24c2317b-e845-4cbc-bb4c-fbb93e51f72e";
std::string  TEST_CHAR_1        =  "770294ed-f345-4f8b-bf3e-063b52d314ab";

std::map<std::string, BLECharacteristic*> charMap;
std::map<std::string, std::string> notifMap;

String payloadName = "MVPayload_Zest";

uint8_t *image_p = image; //Sets pointer to first element in array
int currentLocation = 0;
int largeDataSize = 0;

int latPin = 4;

BLEServer *pServer = NULL;
BLECharacteristic* pTestChar_1;

bool deviceConnected = false;

std::string readWriteNotif(std::string newValue, std::string data= "")
{
  if(data != "")
  {
    notifMap[newValue] = data;
  }
  if (notifMap.find(newValue) != notifMap.end())
  {
    return(notifMap[newValue]);
  }
  return("");
}

class MyServerCallbacks: public BLEServerCallbacks {
  // TODO this doesn't take into account several clients being connected
    
    void onConnect(BLEServer* pServer) {
      BLEDevice::setPower(ESP_PWR_LVL_N14);
      BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
      pAdvertising->start();
      deviceConnected = true;
      Serial.println("I connected!");
    };
    
    void onDisconnect(BLEServer* pServer) {
      Serial.println("I Disconnected!");
      deviceConnected = false;
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
      int packetSize = 500;
      BLEUUID hit = pCharacteristic->getUUID();
      BLEUUID BLETST = BLEUUID(TEST_CHAR_1);
      //Serial.println("Someone wants to read my data: ");
      
      if(hit.equals(BLETST))
      {
         std::string output = readWriteNotif(TEST_CHAR_1);
         notifMap.erase(TEST_CHAR_1);
         if(output != "")
         {
            pCharacteristic->setValue(output);
         }
         Serial.println("You are BLETEST! Nothing special is going to happen!");
         /*if(currentLocation == -1)
         {
           pCharacteristic->setValue("");
         }
         else
         {
           int nextLocation = 0;
           int endLocation = currentLocation+packetSize - 1;
           nextLocation = endLocation + 1;
           if(endLocation >= largeDataSize)
           {
              endLocation = largeDataSize-1;
              nextLocation = -1;
           }
          
           pCharacteristic->setValue(image_p + currentLocation, (endLocation - currentLocation + 1));//message.c_str());
           currentLocation = nextLocation;
         }
         digitalWrite(latPin, HIGH);*/
      }
      else
      {
        Serial.println("Unknown UUID!");  
      }
   }
};

BLECharacteristic* searchCharMap(std::string newValue)
{
  if (charMap.find(newValue) != charMap.end())
  {
    return(charMap[newValue]);
  }
  return(NULL);
}



std::string getValue(std::string data, char separator, int index, bool guard=false)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.at(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    if(guard) //protects against seperator charecters in uncontrolled user entree fields
    {
      strIndex[1] = maxIndex;
    }
    return found > index ? data.substr(strIndex[0], strIndex[1]-strIndex[0]) : "";
}

void sendNotify(BLECharacteristic* chr, int notif = 0)
{

  Serial.println("Setting Notify Value");
  chr->setValue((uint32_t&)notif); 

  chr->notify();
  Serial.println("Notification Sent");
} 


void interpretCommand(std::string input)
{
  std::string modifier = getValue(input, ' ', 0);
  std::string  UUID    = getValue(input, ' ', 1);
  std::string  payload   = getValue(input, ' ' , 2, true); //guard = true, allows for spaces in data
  Serial.print("Modifier: ");
  Serial.println(modifier.c_str());
  Serial.print("UUID: ");
  Serial.println(UUID.c_str());
  Serial.print("Payload: ");
  Serial.println(payload.c_str());
  if ((modifier == "Update") || (modifier == "UpdateN"))
  {
  	Serial.println("Update");
  	//Update Value
  	//Search Char Map for UUID conversion
  	//Update UUID with payload
    BLECharacteristic* out = searchCharMap(TEST_CHAR_1);
    if(out != NULL)
    {
    	//TODO: Need check to see if data needs the largeDataFunction
    	out->setValue(payload); 
      if(modifier == "UpdateN")
      {
        readWriteNotif(UUID.c_str(), payload);
        sendNotify(out);
        
      }
    }
    else
    {
      Serial.println("Improper Syntax!");
    }
  }
  if (modifier == "Read")
  {
  	Serial.println("Read");
    BLECharacteristic* out = searchCharMap(TEST_CHAR_1);
    if(out != NULL)
    {
    	//TODO: Need check to see if data needs the largeDataFunction
    	Serial.println(out->getValue().c_str());
    }
    else
    {
      Serial.println("Improper Syntax!");
    }
  }
  if (modifier == "A")
  {
      //Autodiscover
  }
  if (modifier == "N")
  {
      //Check Notifications
  }
}



void setup()
{
  Serial.begin(115200);                                                         // Initialize serial port
  Serial.println("Starting BLE work!");

  pinMode(latPin, OUTPUT);

  BLEDevice::init(payloadName.c_str());                                                      // Initialize the BLE device
  BLEDevice::setPower(ESP_PWR_LVL_N14);
  BLEServer *pServer = BLEDevice::createServer();                               // Save the BLE device server

  //Creating Services
  BLEService *pTestService = pServer->createService(TEST_SERVICE_UUID);                 // Create the service UUID from the server
  //BLEService *pBestService = pServer->createService(BEST_SERVICE_UUID);                 // Create the service UUID from the server
    
  //Creating Characteristics
  pTestChar_1 = pTestService->createCharacteristic(                                // Create the characteristic UUID for server
                                         BLEUUID(TEST_CHAR_1),
                                         BLECharacteristic::PROPERTY_READ  |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );

  pTestChar_1->setCallbacks(new MyCallbacks()); 

  pTestChar_1->addDescriptor(new BLE2902());

  charMap.insert(std::pair<std::string,BLECharacteristic*>(TEST_CHAR_1, pTestChar_1));

  pTestService->start();                                                            // Start service

  //pBestService->start();   
  
  pServer->setCallbacks(new MyServerCallbacks());                               // Set server callbacks
  
  //Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   // Grab advertisiing service
  pAdvertising->addServiceUUID(TEST_SERVICE_UUID);   
  // pAdvertising->addServiceUUID(BEST_SERVICE_UUID);  
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();                                                // Start advertising
  Serial.println("Setup Complete. We are Advertising!");
}


int isNotZero(int num)
{
  if(num == 0)
  {
    return 0;
  }
  return 1;
}

void sendLargeData(BLECharacteristic* chr, uint8_t *image_p, int len)
{
  Serial.println("Configuring Large Data");
  currentLocation = 0;
  largeDataSize = len;
  int totalTranmissions = (len/TRANSMISSION_SIZE) + isNotZero(len%TRANSMISSION_SIZE);
  Serial.print("Total Transmissions: ");
  Serial.println(totalTranmissions);
  sendNotify(chr, totalTranmissions);
}

void loop() {
  if (deviceConnected) {
     Serial.println("Wow, I am connected!");
     //Serial.println("Sending Notification!");

     //sendLargeData(pTestChar_1, image_p, iLen_int);
  }
  else
  {
     Serial.print(BLEDevice::getAddress().toString().c_str());
     Serial.println(" Device is not Connected");
  } 

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

  delay(120);
}
