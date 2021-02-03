#ifndef STATICS_H
#define STATICS_H

///DEBUG
#define _DEBUG
///DEBUG

///I2C

// WEMOS
// #define PIN_SDA 4
// #define PIN_SCL 5
// #define BUTTON_PIN 16

// ESPCLOCK
#define PIN_SDA 0
#define PIN_SCL 3
#define BUTTON_PIN 2

#define I2C_ADDR 0x27
///I2C

#include <Arduino.h>

class Statics
{
private:
  Statics() {};
  static char* ClientID;
public:
  static char* GetID();
  static int Rfind(char* str, char needle, int endIndex, int startIndex);
};

const byte WifiIcon[4][8] PROGMEM =
{
  {
    B00000,
    B00000,
    B00000,
    B00011,
    B01100,
    B10000,
    B00011,
    B01100,
  },
  {
    B00000,
    B00000,
    B00000,
    B11000,
    B00110,
    B00001,
    B11000,
    B00110,
  },
  {
    B00001,
    B00010,
    B00000,
    B00001,
    B00001,
    B00000,
    B00000,
    B00000,
  },
  {
    B10000,
    B01000,
    B00000,
    B10000,
    B10000,
    B00000,
    B00000,
    B00000,
  }
};

const byte NoWifiIcon[4][8] PROGMEM =
{
  {
    B00000,
    B00000,
    B00000,
    B10011,
    B01100,
    B10100,
    B00011,
    B01101,
  },
  {
    B00000,
    B00000,
    B00000,
    B11000,
    B00110,
    B00001,
    B11000,
    B00110,
  },
  {
    B00001,
    B00010,
    B00000,
    B00001,
    B00001,
    B00000,
    B00000,
    B00000,
  },
  {
    B10000,
    B01000,
    B00100,
    B10010,
    B10001,
    B00000,
    B00000,
    B00000,
  }
};

#endif