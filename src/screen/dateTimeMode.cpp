#include "screen/dateTimeMode.h"

#include <Time.h>

DateTimeMode::DateTimeMode(LiquidCrystal_I2C* lcd) : ScreenMode(lcd)
{

}

bool DateTimeMode::Show()
{
    _lcd->clear();
    _secCnt = 0;

    if (timeStatus() == timeStatus_t::timeSet)
    {
        _lastDay = 255;
        _lastHours = 255;
        _lastMinutes = 255;
        _lastSeconds = 255;
        for (int i = 0; i < 8; i++)
        {
            byte* buf = new byte[8];
            memcpy_P(buf, &CustomChars[i], 8);
            _lcd->createChar(i, buf);
            delete[] buf;
        }

        initState(0, true);
        _changeTime = 50000;
        Update();
    }
    else
    {
        _lcd->print("Time isn`t set!");
        _lcd->setCursor(0, 1);
        _lcd->print("Trying to get...");
        _changeTime = 10000;
        _state = -1;
    }
    return true;
}

void DateTimeMode::Update()
{
    if (_state != -1)
    {
        if (_secCnt > 40) initState(1, false);
        else initState(0, false);

        if (_state == 0)
        {
            if (_lastHours != hour())
            {
                printBigNum(hour() / 10, 0, 0);
                printBigNum(hour() % 10, 3, 0);
                _lastHours = hour();
            }

            if (_lastMinutes != minute())
            {
                printBigNum(minute() / 10, 7, 0);
                printBigNum(minute() % 10, 10, 0);
                _lastMinutes = minute();
            }

            if (_lastSeconds != second())
            {
                _lcd->setCursor(14, 1);
                _lcd->print(second() / 10);
                _lcd->setCursor(15, 1);
                _lcd->print(second() % 10);
                _lastSeconds = second();
            }

        }
        else if (_state == 1)
        {
            if (_lastDay != day())
            {
                _lastDay = day();
                if (_lastDay > 9)
                {
                    printBigNum(day() / 10, 0, 0);
                    printBigNum(day() % 10, 3, 0);
                    _lcd->setCursor(7, 0);
                    _lcd->print("of");
                    _lcd->setCursor(7, 1);
                    _lcd->print(monthStr(month()));
                }
                else
                {
                    printBigNum(day(), 3, 0);
                    _lcd->setCursor(7, 0);
                    _lcd->print("of");
                    _lcd->setCursor(7, 1);
                    _lcd->print(monthStr(month()));
                }
            }
        }
    }
    else
    {
        if (timeStatus() == timeStatus_t::timeSet)
            this->Show();
    }
    _secCnt++;
}
int DateTimeMode::GetChangeTime() { return _changeTime; }
int DateTimeMode::GetUpdateTime() { return 1000; }
OVERRIDE_BACKLIGHT DateTimeMode::OverrideBacklight() { return OVERRIDE_BACKLIGHT::NO_OVERRIDE; }
bool DateTimeMode::MaySkipByButton() { return true; }

void DateTimeMode::printBigNum(int number, int startCol, int startRow)
{
    _lcd->setCursor(startCol, startRow);
    byte* thisNumber = new byte[6];
    for (int cnt = 0; cnt < 6; cnt++) {
        thisNumber[cnt] = pgm_read_byte(&(BigNumbers[number][cnt]));
    }
    for (int cnt = 0; cnt < 3; cnt++) {
        _lcd->print((char)thisNumber[cnt]);
    }
    _lcd->setCursor(startCol, startRow + 1);
    for (int cnt = 3; cnt < 6; cnt++) {
        _lcd->print((char)thisNumber[cnt]);
    }
    delete[] thisNumber;
}

void DateTimeMode::initState(int state, bool force)
{
    if (!force && state == _state)
        return;

    _lcd->clear();
    _state = state;

    if (state == 0)
    {
        _lcd->setCursor(6, 0);
        _lcd->print(".");
        _lcd->setCursor(6, 1);
        _lcd->print(".");

        _lcd->setCursor(13, 0);
        _lcd->print("|");
        _lcd->setCursor(13, 1);
        _lcd->print("|");
    }
}