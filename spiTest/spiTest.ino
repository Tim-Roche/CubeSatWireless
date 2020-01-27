//SPI MASTER (ARDUINO)
//SPI COMMUNICATION BETWEEN TWO ARDUINO 
//CIRCUIT DIGEST

#include<SPI.h>                             //Library for SPI 

const int spiTransferSize = 3;
const int SPI_CS_pin = 15;

void setup (void)
{
  Serial.begin(115200);                   //Starts Serial Communication at Baud Rate 115200 
  
  pinMode(SPI_CS_pin, OUTPUT);
  
  SPI.begin();                            //Begins the SPI commnuication
  SPI.setClockDivider(SPI_CLOCK_DIV8);    //Sets clock for SPI communication at 8 (16/8=2Mhz)
  digitalWrite(SPI_CS_pin,HIGH);                  // Setting SlaveSelect as HIGH (So master doesnt connnect with slave)
}

void tranceiveFromMCU(char TxBuf[spiTransferSize])
{
  SPI.beginTransaction(SPISettings(28000000, MSBFIRST, SPI_MODE0));
  Serial.println("Sending: ");
  digitalWrite(SPI_CS_pin, LOW); //Beginning Transmission
  for(int i = 0; i < spiTransferSize; i++)
  {
    Serial.print(TxBuf[i]);
    TxBuf[i] = SPI.transfer(TxBuf[i]);
  }
  Serial.println();
  digitalWrite(SPI_CS_pin, HIGH); //Terminating Transmission
  SPI.endTransaction();
}

void loop(void)
{
  char TxBuf[spiTransferSize] = "HI";
  tranceiveFromMCU(TxBuf);
  Serial.println("Recieving");
  for(int i = 0; i < spiTransferSize; i++)
  {
    Serial.print(TxBuf[i]);
  }
  Serial.println();
  delay(1000);
}
