#include "BLEDevice.h"
#include <stdint.h>

class charStruct {
public: 
	charStruct(BLERemoteCharacteristic* charecteristic);
	

	BLERemoteCharacteristic* getChar();
	uint8_t getSettings();

	void setSettings(uint8_t settings);

private:
	BLERemoteCharacteristic* m_charecteristic;
	uint8_t m_settings = 0x00;
};
