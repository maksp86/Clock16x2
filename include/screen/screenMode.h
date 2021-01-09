#ifndef SCREEN_SCREENMODE_H
#define SCREEN_SCREENMODE_H

enum OVERRIDE_BACKLIGHT
{
    OVERRIDE_ON,
    OVERRIDE_OFF,
    NO_OVERRIDE
};

class ScreenMode
{
public:
    virtual bool Show() = 0;
    virtual void Update() = 0;
    virtual int GetChangeTime() = 0;
    virtual int GetUpdateTime() = 0;
    virtual OVERRIDE_BACKLIGHT OverrideBacklight() = 0;
    virtual bool MaySkipByButton() = 0;
};

#endif