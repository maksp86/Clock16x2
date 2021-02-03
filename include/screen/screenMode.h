#ifndef SCREEN_SCREENMODE_H
#define SCREEN_SCREENMODE_H

#include <LiquidCrystal_I2C.h>

enum OVERRIDE_BACKLIGHT
{
    OVERRIDE_ON,
    OVERRIDE_OFF,
    NO_OVERRIDE
};

class ScreenMode
{
protected:
    LiquidCrystal_I2C* _lcd;
    bool _fullyInitiated = false;

public:
    ScreenMode(LiquidCrystal_I2C* lcd) { _lcd = lcd; _fullyInitiated = true; };
    ScreenMode() { _fullyInitiated = false; };
    virtual ~ScreenMode() {};
    virtual bool Show() = 0;
    virtual void Init(LiquidCrystal_I2C* lcd) { if (_fullyInitiated) return; _lcd = lcd; _fullyInitiated = true; };
    virtual void Update() = 0;
    virtual int GetChangeTime() = 0;
    virtual int GetUpdateTime() = 0;
    virtual OVERRIDE_BACKLIGHT OverrideBacklight() = 0;
    virtual bool MaySkipByButton() = 0;
};

#endif