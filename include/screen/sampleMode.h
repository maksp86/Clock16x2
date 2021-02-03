#ifndef SCREEN_SAMPLEMODE_H
#define SCREEN_SAMPLEMODE_H

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "screenMode.h"

class SampleMode : public ScreenMode
{
private:

public:
    SampleMode(LiquidCrystal_I2C* lcd);
    ~SampleMode();

    bool Show() override;
    void Update() override;
    int GetChangeTime() override;
    int GetUpdateTime() override;
    OVERRIDE_BACKLIGHT OverrideBacklight() override;
    bool MaySkipByButton() override;
};

const byte Heart[8] PROGMEM = { 0x00, 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00 };

#endif