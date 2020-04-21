#include "parser.h"
#include <string>

std::string getValue(std::string data, char separator, int index, bool guard)
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
