#include "helpers/eepromHelper.h"

#include <EEPROM.h>
char* EepromHelper::Read(int address, int len)
{
  char* str = new char[len];
  for (int i = 0; i < len; i++) str[i] = char(EEPROM.read(address + i)); address += len;
  return str;
}

void EepromHelper::Write(int address, char* data, int len)
{
  EepromHelper::NeedSave = true;
  for (int i = 0; i < len; i++)  EEPROM.write(address + i, data[i]); address += len;
}

void EepromHelper::Commit()
{
  if (EepromHelper::NeedSave)
    EEPROM.commit();
}