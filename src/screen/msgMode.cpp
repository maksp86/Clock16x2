#include "screen/msgMode.h"

#include "helpers/debugHelper.h"
#include "statics.h"

MsgMode::MsgMode(const char* text, bool mayCancel = true) : ScreenMode()
{
    init(text, NULL, NULL, 5000, mayCancel);
    _state = 2;
}

MsgMode::MsgMode(const char* text, const char* title, const byte icon[][8], int showTime, bool mayCancel) : ScreenMode()
{
    byte* tmp = new byte[ICON_SIZE];
    for (int offset = 0, i = 0; i < 4; i++)
    {
#ifdef _DEBUG
        DebugHelper::Log("MsgMode", "start");
        DebugHelper::Log("MsgMode", i);
#endif
        if (memcpy_P(tmp + offset, icon[i], 8))
        {
            offset += 8;
        }
        else
        {
            delete[] tmp;
            tmp = NULL;
            break;
        }
#ifdef _DEBUG
        DebugHelper::Log("MsgMode", "end");
        DebugHelper::Log("MsgMode", i);
#endif
    }
    init(text, title, tmp, showTime, mayCancel);
}

MsgMode::MsgMode(const char* text, const char* title, byte* icon, int showTime, bool mayCancel) : ScreenMode()
{
    init(text, title, icon, showTime, mayCancel);
}

void MsgMode::init(const char* text, const char* title, byte* icon, int showTime, bool mayCancel)
{
#ifdef _DEBUG
    DebugHelper::Log("MsgMode", "construct start");
#endif
    if (icon != NULL)
    {
        lastIndex = 13;
        _icon = icon;
    }
    else
    {
        lastIndex = 15;
        _icon = NULL;
    }
    _autoCancel = mayCancel;

    if (title != NULL)
    {
        int titleLen = strlen(title);
        if (titleLen > lastIndex + 1)
            titleLen = lastIndex + 1;

        _title = new char[titleLen + 1];
        strncpy(_title, title, titleLen);
        _title[titleLen] = '\0';
#ifdef _DEBUG
        DebugHelper::Log("MsgMode", title);
#endif
    }
    else _title = NULL;

    if (text != NULL)
    {
        _textLen = strlen(text);
        _text = new char[_textLen + 1];
        strncpy(_text, text, _textLen);
        _text[_textLen] = '\0';
#ifdef _DEBUG
        DebugHelper::Log("MsgMode", text);
        DebugHelper::Log("MsgMode", _textLen);
#endif
    }
    else _text = NULL;

    _state = 0;
    _offset = 0;
    _showTime = showTime;
    spaceIndex = 0;
    _updTime = 0;
#ifdef _DEBUG
    DebugHelper::Log("MsgMode", "construct end");
#endif
}

MsgMode::~MsgMode()
{
#ifdef _DEBUG
    DebugHelper::Log("MsgMode", "DESTROY");
#endif
    if (_title != NULL)
        delete[]_title;

    if (_text != NULL)
        delete[]_text;

    if (_icon != NULL)
        delete[]_icon;
}

bool MsgMode::Show()
{
#ifdef _DEBUG
    DebugHelper::Log("MsgMode", "SHOW");
#endif

    _lcd->clear();
    if (_icon != NULL)
    {
        int offset = 0;
        while (offset < ICON_SIZE)
        {
            byte* tmp = new byte[8];
            memcpy(tmp, _icon + offset, 8);
            _lcd->createChar(offset / 8, tmp);
            delete[] tmp;
            offset += 8;
        }

        offset = 0;
        for (int row = 0; row < 2; row++)
        {
            _lcd->setCursor(0, row);
            for (int col = 0; col < 2; col++)
            {
                _lcd->print((char)(offset));
                offset++;
            }
        }
    }

    if (_title != NULL)
    {
        _lcd->setCursor(15 - lastIndex, 0);
        _lcd->print(_title);

        if (_textLen <= lastIndex)
        {
            _lcd->setCursor(15 - lastIndex, 1);
            _lcd->print(_text);
            _state = 3;
            _showTime += 2000;
        }
        else
            _state = 2;
    }

    _updTime = 3000;
    return true;
}

void MsgMode::Update()
{
#ifdef _DEBUG
    DebugHelper::Log("MsgMode", "UPDATE");
    DebugHelper::Log("MsgMode", _state);
#endif   
    switch (_state)
    {
    case 2:
    {
        /* show text */
        _lcd->clear();
        _lcd->home();
        if (_lastRow == NULL)
        {
            _lastRow = getNextRow();
#ifdef _DEBUG
            DebugHelper::Log("MsgMode", _lastRow);
#endif   
        }

        if (_lastRow != NULL)
        {
            _lcd->print(_lastRow);
            _lcd->setCursor(0, 1);
        }

        _lastRow = getNextRow();
        if (_lastRow != NULL)
        {
            _lcd->print(_lastRow);
            _showTime += 3000;
        }
        else
        {
            _state++;
        }
        break;
    }
    }
}

int MsgMode::GetChangeTime() { return _showTime; }

int MsgMode::GetUpdateTime() {
    return _updTime;
}

OVERRIDE_BACKLIGHT MsgMode::OverrideBacklight() { return OVERRIDE_BACKLIGHT::NO_OVERRIDE; }

bool MsgMode::MaySkipByButton() { return _buttonSkip; }

char* MsgMode::getNextRow()
{
    if (spaceIndex != -1)
    {
        char* substr = new char[spaceIndex - _offset + 1];
        strncpy(substr, _text + _offset, spaceIndex - _offset);
        substr[spaceIndex - _offset] = '\0';

        _offset += (spaceIndex - _offset) + 1;

        if (spaceIndex > _textLen)
            return NULL;

        if (_offset + lastIndex > _textLen - 1)
            spaceIndex = _textLen + 1;
        else
            spaceIndex = Statics::Rfind(_text, ' ', _offset + lastIndex + 1, _offset);
        return substr;
    }
    else
        return NULL;
}