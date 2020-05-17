//Example Payload for Rowan CubeSat Wireless Interconnect Clinic Project
//2019-2020: Tim Roche, Kenny Wagner, Jake Matteo

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>
#include "payloadSettings.h"

#define RXD2 16
#define TXD2 17

std::map<std::string, BLECharacteristic*> charMap;
std::map<std::string, std::string> notifMap;

int latPin = 22;
int LEDpin = 2;

BLEServer *pServer = NULL;
BLECharacteristic* pTestChar_1;

int count = 0;
int maxCount = 1000;
int currentLocation = 0;
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
    
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
      esp_ble_conn_update_params_t conn_params = {0};
      memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
      conn_params.latency = 0; //number of skippable connection events
      conn_params.max_int = 0x06; // max_int = 6*1.25ms = 7ms
      conn_params.min_int = 0x06; // min_int = 6*1.25ms = 7ms
      conn_params.timeout = 200;  // timeout = 200*10ms = 2000ms
      //start sent the update connection parameters to the peer device.
      esp_ble_gap_update_conn_params(&conn_params);

      BLEDevice::setPower(ESP_PWR_LVL_N14); //ESP_PWR_LVL_N14);
      BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
      pAdvertising->start();
      deviceConnected = true;
      digitalWrite(LEDpin,1);
      //Serial.println("I connected!");
    };
    
    void onDisconnect(BLEServer* pServer) {
      //Serial.println("I Disconnected!");
      digitalWrite(LEDpin,0);
      deviceConnected = false;
    }  
};


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string uuid = pCharacteristic->getUUID().toString();
      std::string value = pCharacteristic->getValue();
  }


    void onRead(BLECharacteristic* pCharacteristic) {
      int packetSize = 500;
      BLEUUID hit = pCharacteristic->getUUID();
      BLEUUID BLETEMP = BLEUUID(TEMP_CHAR);
      BLEUUID BLELIGHT= BLEUUID(LIGHT_CHAR);

      ////Serial.println("Someone wants to read my data: ");
      
      if(hit.equals(BLETEMP))
      {
         Serial.println("Sucessful Write! BLE TEMP!");
         std::string output = readWriteNotif(TEMP_CHAR);
         notifMap.erase(TEMP_CHAR);
         if(output != "")
         {
            pCharacteristic->setValue(output);
         }
      }
      if(hit.equals(BLELIGHT))
      {
         Serial.println("Sucessful Write! BLE LIGHT!");
         std::string output = readWriteNotif(LIGHT_CHAR);
         notifMap.erase(LIGHT_CHAR);
         if(output != "")
         {
            pCharacteristic->setValue(output);
         }
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

  //Serial.println("Setting Notify Value");
  chr->setValue((uint32_t&)notif); 

  chr->notify();
  //Serial.println("Notification Sent");
} 

void interpretCommand(std::string input)
{
  std::string modifier = getValue(input, ' ', 0);
  std::string  UUID    = getValue(input, ' ', 1);
  std::string  payload   = getValue(input, ' ' , 2, true); //guard = true, allows for spaces in data
  //Serial.print("Modifier: ");
  //Serial.println(modifier.c_str());
  //Serial.print("UUID: ");
  //Serial.println(UUID.c_str());
  //Serial.print("Payload: ");
  //Serial.println(payload.c_str());
  if ((modifier == "Update") || (modifier == "UpdateN"))
  {
    //Serial.println("Update");
    //Update Value
    //Search Char Map for UUID conversion
    //Update UUID with payload
    BLECharacteristic* out = searchCharMap(UUID.c_str());
    if(out != NULL)
    {
      //TODO: Need check to see if data needs the largeDataFunction
      digitalWrite(latPin, 1);
      out->setValue(payload); 
      if(modifier == "UpdateN")
      {
        readWriteNotif(UUID.c_str(), payload);
        sendNotify(out);
      }
      Serial.println("[Complete]");
      digitalWrite(latPin, 0);
    }
    else
    {
      Serial.println("[Invalid Syntax]");
      Serial.println("Improper Syntax!");
    }
  }
  if (modifier == "Read")
  {
    //Serial.println("Read");
    BLECharacteristic* out = searchCharMap(UUID.c_str());
    if(out != NULL)
    {
      //TODO: Need check to see if data needs the largeDataFunction
      std::string outputString = out->getValue();
      //Serial.println(outputString.c_str());
      //Serial2.println(outputString.c_str());
    }
    else
    {
      Serial2.println("[Invalid Syntax]");
      Serial.println("Improper Syntax!");
    }
  }
  if(modifier == "Echo")
  {
    Serial2.println(UUID.c_str());
  }
}

//Initilizes UART between PC -> ESP
//and between ESP -> MCU
void init_UART()
{
  Serial.begin(115200);   
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //Serial.println("Serial Txd is on pin: "+String(TX));
  //Serial.println("Serial Rxd is on pin: "+String(RX));
}

void setup()
{
  init_UART();
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin,0);
  pinMode(latPin, OUTPUT);
  digitalWrite(latPin, 0);
  delay(250);
  digitalWrite(latPin, 1);
  delay(1000);
  digitalWrite(latPin, 0);
  Serial.println("Payload");
  BLEDevice::init(payloadName.c_str());                                                      // Initialize the BLE device
  //BLEDevice::setPower(ESP_PWR_LVL_N14);
  BLEServer *pServer = BLEDevice::createServer();                               // Save the BLE device server

  //Creating Services
  BLEService *pTestService = pServer->createService(SENSOR_SERVICE_UUID);                 // Create the service UUID from the server
  pTestChar_1 = pTestService->createCharacteristic(                                // Create the characteristic UUID for server
                                         BLEUUID(TEMP_CHAR),
                                         BLECharacteristic::PROPERTY_READ  |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  pTestChar_1->setCallbacks(new MyCallbacks()); 
  pTestChar_1->addDescriptor(new BLE2902());
  
  charMap.insert(std::pair<std::string,BLECharacteristic*>(TEMP_CHAR, pTestChar_1));

  pTestChar_1 = pTestService->createCharacteristic(                                // Create the characteristic UUID for server
                                         BLEUUID(LIGHT_CHAR),
                                         BLECharacteristic::PROPERTY_READ  |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY |
                                         BLECharacteristic::PROPERTY_INDICATE
                                       );
  pTestChar_1->setCallbacks(new MyCallbacks()); 
  pTestChar_1->addDescriptor(new BLE2902());
  
  charMap.insert(std::pair<std::string,BLECharacteristic*>(LIGHT_CHAR, pTestChar_1));


  pTestService->start();                                                            // Start service
  pServer->setCallbacks(new MyServerCallbacks());                               // Set server callbacks
  
  //Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   // Grab advertisiing service
  pAdvertising->addServiceUUID(SENSOR_SERVICE_UUID);   
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();                                                // Start advertising
  //Serial.println("Setup Complete. We are Advertising!");
}


int isNotZero(int num)
{
  if(num == 0)
  {
    return 0;
  }
  return 1;
}

void checkForCommands()
{
  int incomingByte = 0; // incoming byte from serial input
  char c;
  std::string output = "";

  while (Serial.available() > 0) 
  {
    incomingByte = Serial.read();
    c = (char) incomingByte;
    //Serial.print(c);
    output += c; 
  }
    if((output[0] != 'P')&&(output != ""))
    {
      count = 0;
      maxCount = 0;
      interpretCommand(output);
    }
   if((1==1)||(output[0] == 'P')||((count>0)&&(count<maxCount)))
    {
      std::string modifier = getValue(output, ' ', 1);
      if(modifier != "")
      {
        maxCount = atoi(modifier.c_str());
      }
      output = "UpdateN 770294ed-f345-4f8b-bf3e-063b52d314ab 1234 ";
      interpretCommand(output);
      Serial.print(output.c_str());
      delay(50);
      Serial.print(" | TX Num:");
      Serial.print(count);
      Serial.print("/");
      Serial.print(maxCount);
      Serial.println();
      count++;
  }
}

//void sendLargeData(BLECharacteristic* chr, uint8_t *image_p, int len)
//{
//  //Serial.println("Configuring Large Data");
//  currentLocation = 0;
//  largeDataSize = len;
//  int totalTranmissions = (len/TRANSMISSION_SIZE) + isNotZero(len%TRANSMISSION_SIZE);
//  //Serial.print("Total Transmissions: ");
//  //Serial.println(totalTranmissions);
//  sendNotify(chr, totalTranmissions);
//}

void loop() {
  if (deviceConnected) {
     ////Serial.println("Wow, I am connected!");
     ////Serial.println("Sending Notification!");

     //sendLargeData(pTestChar_1, image_p, iLen_int);
     checkForCommands();
  }
  else
  {
     Serial.print(BLEDevice::getAddress().toString().c_str());
     Serial.println(" Device is not Connected");
  } 
  //checkForCommands();
  delay(120);
}
