int payload = 15;
int CDH = 2;
bool done = false;
bool toggle = false;
void setup() {
  pinMode(payload, INPUT);
  pinMode(CDH, INPUT);
  Serial.begin(115200);
  Serial.println("Waiting...");
}


unsigned long startingTime = 1;
unsigned long endingTime = 0;

void loop() {

  while(!done)
  {
    if((!toggle) && (digitalRead(payload)))
    {
      startingTime = millis();
      toggle = true;
    }
    if((toggle) && (digitalRead(CDH)))
    {
      endingTime = millis();
      done = true;
    }
  }
  Serial.println(endingTime - startingTime);
}
