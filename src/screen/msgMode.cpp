#include "screen/msgMode.h"

#include "statics.h"

MsgMode::MsgMode(const char* text)
{
    _icon = NULL;
    _title = NULL;
    offset = 0;
    spaceIndex = 0;

    _autoCancel = true;

    _textLen = strlen(text);
    _text = new char[_textLen + 1];
    strncpy(_text, text, _textLen);
    lastIndex = 15;
}

MsgMode::MsgMode(const char* text)
{
    _icon = NULL;
    _title = NULL;
    offset = 0;
    spaceIndex = 0;

    _autoCancel = true;

    _textLen = strlen(text);
    _text = new char[_textLen + 1];
    strncpy(_text, text, _textLen);
    lastIndex = 15;
}

MsgMode::~MsgMode()
{
    if (_title != NULL)
        delete _title;

    if (_text != NULL)
        delete _text;

    if (_icon != NULL)
        delete _icon;
}

bool MsgMode::Show()
{

}

void MsgMode::Update()
{
    switch (_state)
    {
    case 0:
        /* show image */
        break;

    case 1:
        /* show title */
        break;

    case 2:
        /* show text */
        break;
    }
}

int MsgMode::GetChangeTime() { return _showTime; }

int MsgMode::GetUpdateTime() { return 1000; }

OVERRIDE_BACKLIGHT MsgMode::OverrideBacklight() { return OVERRIDE_BACKLIGHT::NO_OVERRIDE; }

bool MsgMode::MaySkipByButton() { }

char* MsgMode::getNextRow()
{
    if (spaceIndex != -1)
    {
        char* substr = new char[spaceIndex - offset + 1];
        strncpy(substr, _text + offset, spaceIndex - offset);
        substr[spaceIndex - offset] = '\0';

        offset += (spaceIndex - offset) + 1;

        if (spaceIndex > _textLen)
            return NULL;

        if (offset + lastIndex > _textLen - 1)
            spaceIndex = _textLen + 1;
        else
            spaceIndex = Statics::Rfind(_text, ' ', offset + lastIndex + 1, offset);
        return substr;
    }
    else return NULL;
}