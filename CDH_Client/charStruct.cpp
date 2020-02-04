#include "charStruct.h"
#include <stdint.h>
#include "BLEDevice.h"

charStruct::charStruct()
{
  m_settings = 0x00;
}

charStruct::charStruct(BLERemoteCharacteristic* charecteristic)
{
	m_charecteristic = charecteristic;
	m_settings = 0x00;
}

charStruct::charStruct(uint8_t settings)
{
	m_settings = settings;
}


BLERemoteCharacteristic* charStruct::getCharecteristic()
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

void charStruct::setCharecteristic(BLERemoteCharacteristic* charecteristic)
{
	m_charecteristic = charecteristic;
}
