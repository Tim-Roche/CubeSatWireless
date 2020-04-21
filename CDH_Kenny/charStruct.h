#include "BLEDevice.h"
#include <stdint.h>

class charStruct {
public: 
  charStruct();
	charStruct(BLERemoteCharacteristic* charecteristic, std::string UUID);
	charStruct(uint8_t settings, std::string UUID);
	

	BLERemoteCharacteristic* getCharecteristic();
	uint8_t getSettings();
  std::string getUUID();
  
  void setUUID(std::string UUID);
  void setCharecteristic(BLERemoteCharacteristic* charecteristic);
	void setSettings(uint8_t settings);

private:
	BLERemoteCharacteristic* m_charecteristic;
	uint8_t m_settings = 0x00;
  std::string m_UUID = "";
};
