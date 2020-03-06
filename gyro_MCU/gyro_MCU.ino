#define RXD2 16
#define TXD2 17

int latPin = 23;

int count =1;


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!

// i2c
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();

// You can also use software SPI
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(13, 12, 11, 10, 9);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(10, 9);

void setupSensor()
{
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
}

void setup() 
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  pinMode(latPin, OUTPUT);
  digitalWrite(latPin, 0);
  Serial.println("ESP32 Command Tester for CubeSatWireless");

  
  Serial.println("LSM raw read demo");
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS0. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS0 9DOF");
  Serial.println("");
  Serial.println("");
}

void sendAccel()
{
  Serial2.print("Accel X: "); Serial2.print((int)lsm.accelData.x); Serial2.print(" ");
  Serial2.print("Y: "); Serial2.print((int)lsm.accelData.y);       Serial2.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.accelData.z);       Serial2.print(" ");
}

void loop() 
{
  lsm.read();


  Serial.print("Mag X: "); Serial.print((int)lsm.magData.x);     Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.magData.y);         Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.magData.z);       Serial.print(" ");
  Serial.print("Gyro X: "); Serial.print((int)lsm.gyroData.x);   Serial.print(" ");
  Serial.print("Y: "); Serial.print((int)lsm.gyroData.y);        Serial.print(" ");
  Serial.print("Z: "); Serial.println((int)lsm.gyroData.z);      Serial.println(" ");
  Serial.print("Temp: "); Serial.print((int)lsm.temperature);    Serial.println(" ");
  delay(200);
}

void loop() { //Choose Serial1 or Serial2 as required
  
  int incomingByte = 0; // incoming byte from serial input
  char c;
  String output = "";
  String command = "";
  while (Serial.available() > 0) 
  {
    incomingByte = Serial.read();
    c = (char) incomingByte;
    output += c; 
  }
  if((output.charAt(0) == 'T')||(count>1))
  {
 
      command = "UpdateN 770294ed-f345-4f8b-bf3e-063b52d314ab ";
      digitalWrite(latPin, 1);

      delay(500);
      digitalWrite(latPin, 0);
      Serial.println(count);
      count++;
  }
  if(output.charAt(0) == 'L')
  {
      command = "UpdateN 5276084c-0f40-4e15-be7f-9ba118ccfdd9 ";
      digitalWrite(latPin, 1);

      delay(500);
      digitalWrite(latPin, 0);
  }
  output = "";
  delay(100);
  incomingByte = 0; // incoming byte from serial input
  c = ' ';
  output = "";
  
  while (Serial2.available() > 0) 
  {
    incomingByte = Serial2.read();
    c = (char) incomingByte;
    output += c; 
  }
  if(output != "")
  {
    digitalWrite(1, latPin);
    digitalWrite(0, latPin);
  }
  delay(50);
}
