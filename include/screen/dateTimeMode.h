#ifndef SCREEN_TIMEMODE_H
#define SCREEN_TIMEMODE_H

#include "screenMode.h"
#include "Arduino.h"

#include <LiquidCrystal_I2C.h>

class DateTimeMode : public ScreenMode
{
private:
  int _changeTime;
  int _state;

  int _lastHours;
  int _lastMinutes;
  int _lastSeconds;
  int _lastDay;

  int _secCnt;

  LiquidCrystal_I2C* _lcd;

  void initState(int state);
  void printBigNum(int number, int startCol, int startRow);

public:
  DateTimeMode(LiquidCrystal_I2C* lcd);
  bool Show();
  void Update();
  int GetChangeTime();
  int GetUpdateTime();
  OVERRIDE_BACKLIGHT OverrideBacklight();
  bool MaySkipByButton();

};
const byte CustomChars[8][8] PROGMEM = {
  {31, 31, 31, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 31, 31, 31},
  {31, 0, 0, 0, 0, 0, 0, 31},
  {0, 0, 0, 0, 0, 0,  0, 31},
  {31, 31, 31, 31, 31, 31, 15, 7},
  {28, 30, 31, 31, 31, 31, 31, 31},
  {31, 31, 31, 31, 31, 31, 30, 28},
  {7, 15, 31, 31, 31, 31, 31, 31}
};
const byte BigNumbers[10][6] PROGMEM = {
  {7, 0, 5, 4, 1, 6},         //0
  {0, 5, 254, 1, 255, 1},     //1
  {0, 2, 5, 7, 3, 1},         //2
  {0, 2, 5, 1, 3, 6},         //3
  {7, 3, 255, 254, 254, 255}, //4
  {7, 2, 0, 1, 3, 6},         //5
  {7, 2, 0, 4, 3, 6},         //6
  {0, 0, 5, 254, 7, 254},   //7
  {7, 2, 5, 4, 3, 6},         //8
  {7, 2, 5, 1, 3, 6}         //9
};

#endif