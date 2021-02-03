#ifndef __PROGRESSBARMODE_H__
#define __PROGRESSBARMODE_H__

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "screenMode.h"

class ProgressBarMode : public ScreenMode
{
private:
    int* _ptr;
    char* _text;
    int lastValue;
public:
    ProgressBarMode(const char* text, int* progress);
    ~ProgressBarMode();
    bool Show() override;
    void Update() override;
    int GetChangeTime() override;
    int GetUpdateTime() override;
    OVERRIDE_BACKLIGHT OverrideBacklight() override;
    bool MaySkipByButton() override;
};

#define FREEPROGRESSCELL 0
#define FULLPROGRESSCELL 1
#define LEFTBORDERCELL 2
#define RIGHTBORDERCELL 3
const byte ProgressCellStates[4][8] PROGMEM
{
{
B11111,
B00000,
B00000,
B00000,
B00000,
B00000,
B00000,
B11111
},
{
B11111,
B00000,
B11111,
B11111,
B11111,
B11111,
B00000,
B11111
},
{
B01111,
B10000,
B10000,
B10000,
B10000,
B10000,
B10000,
B01111
},
{
B11110,
B00001,
B00001,
B00001,
B00001,
B00001,
B00001,
B11110
}
};

#endif //__PROGRESSBARMODE_H__