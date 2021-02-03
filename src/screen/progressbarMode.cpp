#include "screen/progressbarMode.h"

#include "helpers/debugHelper.h"
#include "statics.h"

ProgressBarMode::ProgressBarMode(const char* text, int* progress) : ScreenMode()
{
    int textLen = strlen(text);
    if (textLen > 16)
        textLen = 16;

    _text = new char[textLen + 1];
    strncpy(_text, text, textLen);
    _text[textLen] = '\0';

    _ptr = progress;
}

ProgressBarMode::~ProgressBarMode()
{

}

bool ProgressBarMode::Show()
{
    _lcd->clear();

    for (int i = 0; i < 4; i++)
    {
        byte* buf = new byte[8];
        memcpy_P(buf, &ProgressCellStates[i], 8);
        _lcd->createChar(i, buf);
        delete[] buf;
    }

    _lcd->setCursor(0, 0);
    _lcd->print(_text);

    _lcd->setCursor(0, 1);

    _lcd->print((char)LEFTBORDERCELL);
    for (int i = 1; i < 11; i++)
    {
        _lcd->print((char)FREEPROGRESSCELL);
    }
    _lcd->print((char)RIGHTBORDERCELL);

    _lcd->print(F(" 00%"));
    return true;
}

void ProgressBarMode::Update()
{
    if (*_ptr != lastValue)
    {
        lastValue = *_ptr;

#ifdef _DEBUG
        DebugHelper::Log("ProgressBarMode", "new value");
        DebugHelper::Log("ProgressBarMode", lastValue);
#endif

        int pos = lastValue / 10;
        if (pos > 9)
            pos = 9;
        _lcd->setCursor(pos + 1, 1);
        _lcd->print((char)FULLPROGRESSCELL);

        char* buf = new char[4];
        sprintf(buf, "%02d%%", lastValue);
        _lcd->setCursor(13, 1);
        _lcd->print(buf);
        delete[] buf;
    }
}

int ProgressBarMode::GetChangeTime()
{
    return -1;
}

int ProgressBarMode::GetUpdateTime()
{
    return 500;
}

OVERRIDE_BACKLIGHT ProgressBarMode::OverrideBacklight()
{
    return OVERRIDE_BACKLIGHT::OVERRIDE_ON;
}

bool ProgressBarMode::MaySkipByButton()
{
    return false;
}