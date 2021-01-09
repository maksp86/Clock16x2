#ifndef HELPERS_EEPROM_HELPER_H
#define HELPERS_EEPROM_HELPER_H

#include <Arduino.h>

class EepromHelper
{

public:
  static char* Read(int address, int len);
  static void Write(int address, char* data, int len);
  static void Commit();

private:
  EepromHelper() {};
  static bool NeedSave;
};

#endif