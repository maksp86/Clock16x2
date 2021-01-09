#ifndef STATICS_H
#define STATICS_H

///DEBUG
#define _DEBUG
///DEBUG


///I2C
#define PIN_SDA D2
#define PIN_SCL D1
#define BUTTON_PIN D0
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
    static int Rfind(char* str, char needle, int endIndex, int startIndex = 0);
};

#endif