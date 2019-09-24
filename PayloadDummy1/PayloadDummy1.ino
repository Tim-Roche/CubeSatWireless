#define PHOTOPIN 32
#define TEMPPIN 35
#define LEDPIN 16

int photoValue = 0;
int tempValue = 0;
int LEDValue = 1024;
int LEDChannel = 0;

int freq = 5000;
int resolution = 8;

void setup() {
  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  ledcSetup(LEDChannel, freq, resolution);
  ledcAttachPin(LEDPIN, LEDChannel);
}

void loop() {
  photoValue = analogRead(PHOTOPIN);    // read the value from the sensor
  tempValue  = analogRead(TEMPPIN);
  int resolution = 128;
  ledcWrite(LEDChannel, resolution);
  Serial.print("Photo Value: ");
  Serial.print(photoValue);
  Serial.print("  ");
  Serial.print("Temp Value: ");
  Serial.println(tempValue);
  delay(100);
}
