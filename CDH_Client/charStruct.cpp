#include "charStruct.h"
#include <stdint.h>
#include "BLEDevice.h"

charStruct::charStruct(BLERemoteCharacteristic* charecteristic)
{
	m_charecteristic = charecteristic;
}

BLERemoteCharacteristic* charStruct::getChar()
{
	return(m_charecteristic);
}

uint8_t charStruct::getSettings()
{
	return(m_settings);
}

void charStruct::setSettings(uint8_t settings)
{
	m_settings = settings;
}