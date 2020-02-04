#include "BLEDevice.h"
#include <stdint.h>

class charStruct {
public: 
  charStruct();
	charStruct(BLERemoteCharacteristic* charecteristic);
	charStruct(uint8_t settings);
	

	BLERemoteCharacteristic* getCharecteristic();
	uint8_t getSettings();
  void setCharecteristic(BLERemoteCharacteristic* charecteristic);
  
	void setSettings(uint8_t settings);

private:
	BLERemoteCharacteristic* m_charecteristic;
	uint8_t m_settings = 0x00;
};
