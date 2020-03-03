#include <string>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
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
    Serial.println(output);
    interpretCommand(output.c_str());
  }

  delay(100);
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

void interpretCommand(std::string input)
{
  std::string modifier = getValue(input, ' ', 0);
  std::string  UUID = getValue(input, ' ', 1);
  std::string  datas = getValue(input, ' ' , 2, true);
  Serial.print("Modifier: ");
  Serial.println(modifier.c_str());
  Serial.print("UUID: ");
  Serial.println(UUID.c_str());
  Serial.print("Datas: ");
  Serial.println(datas.c_str());
  if (modifier.c_str() == "Update")
  {
    Serial.println("Update");
  }
  if (modifier == "Read")
  {
    Serial.println("Read");
  }
  if (modifier == "A")
  {
    Serial.println("A!");
  }
  if (modifier == "N")
  {
      //Check Notifications
      Serial.println("N!");
  }
}
