#include "screen/sampleMode.h"
#include "helpers/debugHelper.h"

#include "statics.h"


SampleMode::SampleMode(LiquidCrystal_I2C* lcd) : ScreenMode(lcd)
{
}

SampleMode::~SampleMode()
{
#ifdef _DEBUG
    DebugHelper::Log("sampleMode", "destroy");
#endif
}

bool SampleMode::Show()
{
#ifdef _DEBUG
    DebugHelper::Log("sampleMode", "show");
#endif
    _lcd->clear();

    ///CREATECHAR MUST BE BEFORE ANY WRITING TO DISPLAY (EXCLUDING CLEAR)!!!!!!
    byte* heartIcon = new byte[8];
    memcpy_P(heartIcon, &Heart, 8);
    _lcd->createChar(0, heartIcon);
    delete[] heartIcon;
    ///CREATECHAR MUST BE BEFORE ANY WRITING TO DISPLAY (EXCLUDING CLEAR)!!!!!!

    _lcd->home();
    _lcd->print(F("This is a sample"));
    _lcd->setCursor(0, 1);
    delay(500);

    _lcd->print(F("I love "));
    _lcd->write(0);
    _lcd->print(F(" coding"));

    delay(1000);
    _lcd->setCursor(0, 1);
    _lcd->print(F("I hate c++      "));

#ifdef _DEBUG
    DebugHelper::Log("sampleMode", "shown");
#endif
    return true;
}

void SampleMode::Update() {}
int SampleMode::GetChangeTime() { return 10000; }
int SampleMode::GetUpdateTime() { return 1000; }
OVERRIDE_BACKLIGHT SampleMode::OverrideBacklight() { return OVERRIDE_BACKLIGHT::OVERRIDE_ON; }
bool SampleMode::MaySkipByButton() { return true; }