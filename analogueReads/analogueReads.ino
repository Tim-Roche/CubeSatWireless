int tempPin =  34; // potentiometer wiper (middle terminal) connected to analog pin 3
int lightPin = 35;
int lightReading;
int tempReading;
                    // outside leads to ground and +5V
int val = 0;  // variable to store the value read

void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {
  lightReading = analogRead(lightPin);  // read the input pin
  tempReading = analogRead(tempPin);
  Serial.print("Light Reading: ");          // debug value
  Serial.print(lightReading);
  Serial.print(" | Temp Reading: ");
  Serial.println(tempReading);
}
