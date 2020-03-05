/*
 * There are three serial ports on the ESP known as U0UXD, U1UXD and U2UXD.
 * 
 * U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
 * U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
 * U2UXD is unused and can be used for your projects.
 * 
*/

#define RXD2 16
#define TXD2 17

int latPin = 23;

int tempPin =  32; // potentiometer wiper (middle terminal) connected to analog pin 3
int lightPin = 35;
int lightReading;
int tempReading;
int count =1;

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //Serial.println("Serial Txd is on pin: "+String(TX));
  //Serial.println("Serial Rxd is on pin: "+String(RX));
  pinMode(latPin, OUTPUT);
  digitalWrite(latPin, 0);
  Serial.println("ESP32 Command Tester for CubeSatWireless");
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
    //Serial.print(c);
    output += c; 
  }
  if((output.charAt(0) == 'T')||(count>1))
  {
 
      command = "UpdateN 770294ed-f345-4f8b-bf3e-063b52d314ab ";
      tempReading = analogRead(tempPin);  // read the input pin
      digitalWrite(latPin, 1);
      Serial2.print(command);
      Serial2.print(tempReading);
      Serial2.print(" ");
      //Serial.print(command);
      //Serial.print(tempReading);
      delay(500);
      digitalWrite(latPin, 0);
      Serial.println(count);
      count++;
  }
  if(output.charAt(0) == 'L')
  {
      command = "UpdateN 5276084c-0f40-4e15-be7f-9ba118ccfdd9 ";
      lightReading = analogRead(lightPin);  // read the input pin
      digitalWrite(latPin, 1);
      Serial2.print(command);
      Serial2.print(lightReading);
      Serial2.print(" ");
      //Serial.print(command);
      //Serial.println(lightReading);
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
    //Serial.print("Recieved: ");
    //Serial.println(output);
    digitalWrite(0, latPin);
  }
  delay(50);
}
