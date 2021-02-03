#ifndef SCREEN_MSGMODE_H
#define SCREEN_MSGMODE_H

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "screenMode.h"

class MsgMode : public ScreenMode
{
private:

    char* _title;
    char* _text;
    char* _lastRow;
    byte* _icon;

    bool _autoCancel;
    bool _buttonSkip;

    int _textLen;
    int _showTime;
    int _updTime;
    int _state;
    int _offset;
    int lastIndex;
    int spaceIndex;

    char* getNextRow();
    void init(const char* text, const char* title, byte* icon, int showTime, bool mayCancel);
public:
    /**
     * @param text Text to show
     * @param mayCancel Is message may be cancelled by a buttton
     */
    MsgMode(const char* text, bool mayCancel);

    /**
    * @param text Text to show
    * @param title Title of message
    * @param icon byte[4][8] from PROGMEM!!!!
    * @param showTime Time to auto cancel, -1 to disable auto cancel
    * @param mayCancel Is message may be cancelled by a buttton
    */
    MsgMode(const char* text, const char* title, const byte icon[][8], int showTime, bool mayCancel);

    /**
     * @param text Text to show
     * @param title Title of message
     * @param icon 10*16, 32 bytes, NOT FROM PROGMEM
     * @param showTime Time to auto cancel, -1 to disable auto cancel
     * @param mayCancel Is message may be cancelled by a buttton
     */
    MsgMode(const char* text, const char* title, byte* icon, int showTime, bool mayCancel);
    ~MsgMode();
    bool Show() override;
    void Update() override;
    int GetChangeTime() override;
    int GetUpdateTime() override;
    OVERRIDE_BACKLIGHT OverrideBacklight() override;
    bool MaySkipByButton() override;
};

static int ICON_SIZE = 32;

#endif