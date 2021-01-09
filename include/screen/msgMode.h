#ifndef SCREEN_MSGMODE_H
#define SCREEN_MSGMODE_H

#include "Arduino.h"
#include "screenMode.h"

class MsgMode : public ScreenMode
{
private:
    char* _title;
    byte* _icon;

    char* _text;
    int _textLen;

    bool _autoCancel;

    int _showTime;

    int _state;

    int offset;
    int lastIndex;
    int spaceIndex;
    char* getNextRow();
public:
    MsgMode(const char* text);
    ~MsgMode();
    bool Show();
    void Update();
    int GetChangeTime();
    int GetUpdateTime();
    OVERRIDE_BACKLIGHT OverrideBacklight();
    bool MaySkipByButton();
};

#endif