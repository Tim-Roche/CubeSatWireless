int tempPin =  34; // potentiometer wiper (middle terminal) connected to analog pin 3
int lightPin = 35;
                    // outside leads to ground and +5V
int val = 0;  // variable to store the value read

void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {
  val = analogRead(lightPin);  // read the input pin
  Serial.println(val);          // debug value
}
