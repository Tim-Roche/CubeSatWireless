#include "charStruct.h"
#include <stdint.h>
#include "BLEDevice.h"

charStruct::charStruct()
{
  m_settings = 0x00;
}

charStruct::charStruct(BLERemoteCharacteristic* charecteristic, std::string UUID = "")
{
	m_charecteristic = charecteristic;
	m_settings = 0x00;
  m_UUID = UUID;
}

charStruct::charStruct(uint8_t settings, std::string UUID="")
{
	m_settings = settings;
  m_UUID = UUID;
}

BLERemoteCharacteristic* charStruct::getCharecteristic()
{
	return(m_charecteristic);
}


std::string charStruct::getUUID()
{
  return(m_UUID);
}
  
void charStruct::setUUID(std::string UUID)
{
  m_UUID = UUID;
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
