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


void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //Serial.println("Serial Txd is on pin: "+String(TX));
  //Serial.println("Serial Rxd is on pin: "+String(RX));
  Serial.println("ESP32 Command Tester for CubeSatWireless");
}


void loop() { //Choose Serial1 or Serial2 as required
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
    Serial2.print(output.c_str());
  }
  output = "";
  delay(100);
  incomingByte = 0; // incoming byte from serial input
  c = ' ';
  output = "";

  //Serial.println(Serial2.available());
  
  while (Serial2.available() > 0) 
  {
    incomingByte = Serial2.read();
    c = (char) incomingByte;
    output += c; 
  }
  if(output != "")
  {
    Serial.print("Recieved: ");
    Serial.println(output);
  }
  delay(50);
}
