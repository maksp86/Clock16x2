#include "workers/displayWorker.h"

#include <LiquidCrystal_I2C.h>
#include <Time.h>

#include "statics.h"
#include "helpers/debugHelper.h"

#include "screen/screenMode.h"
#include "screen/dateTimeMode.h"
#include "screen/sampleMode.h"

const char* BACKLIGHT_ON_TIME_KEY = "bs";
const char* BACKLIGHT_OFF_TIME_KEY = "be";

DisplayWorker::DisplayWorker()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "init");
#endif

    pinMode(BUTTON_PIN, INPUT);

    screenModes = (ScreenMode**)malloc(sizeof(ScreenMode*));
    _modesCount = 0;
    _allocatedCount = 1;

    _currentModeIndex = 0;
    _backlightUpdateTimer = 0;
    _modeUpdateTimer = 0;
    _changeModeTimer = 0;

    _backlightTime = NULL;
    //_button = (button*)malloc(sizeof(button));
    _button = new button;
    _onbuttonEvent = NULL;

    lcd = new LiquidCrystal_I2C(I2C_ADDR, 16, 2, LCD_5x8DOTS);
    lcd->begin(PIN_SDA, PIN_SCL);

    lcd->backlight();
    lcd->home();
    lcd->print(F("Booting..."));
    lcd->setCursor(0, 1);
    delay(500);
    lcd->print(F("maksp86tech"));
    delay(500);
    lcd->print(F(" 2020"));

    initModes();
}

DisplayWorker::DisplayWorker(configHelper* ch) : DisplayWorker()
{
    config = ch;
}

DisplayWorker::~DisplayWorker()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "destroy");
#endif

    if (_modesCount > 0)
    {
        for (int i = 0; i < _modesCount; i++)
        {
            delete screenModes[i];
        }
        delete[] screenModes;
    }

    if (_backlightTime != NULL)
        delete _backlightTime;

    if (_button != NULL)
        delete _button;
}

InternetNeedState DisplayWorker::InternetNeed() { return InternetNeedState::NO_NEED; }

char* DisplayWorker::Name() { return "DisplayWorker"; }

void DisplayWorker::ShowMode(int index)
{
#ifdef _DEBUG
    DebugHelper::Log(this, "Showing mode ");
    DebugHelper::Log(this, index);
#endif
    if (index >= 0 && index < _modesCount)
    {
        _currentModeIndex = index;
        _currMode = screenModes[_currentModeIndex];
        _currMode->Show();

        _changeModeTimer = millis();
        _modeUpdateTimer = 0;
    }
#ifdef _DEBUG
    else
    {
        DebugHelper::Log(this, "Cannot show mode ");
        DebugHelper::Log(this, index);
    }
#endif
}

void DisplayWorker::ShowMode(ScreenMode* customMode)
{
    if (customMode != NULL)
    {
#ifdef _DEBUG
        DebugHelper::Log(this, "Showing custom mode ");
#endif
        if (_currMode != NULL && _currentModeIndex == -1)
        {
#ifdef _DEBUG
            DebugHelper::Log(this, "deleting ");
#endif
            delete _currMode;
        }
        _currMode = customMode;
        _currentModeIndex = -1;
        _currMode->Init(lcd);
        _currMode->Show();
        _changeModeTimer = millis();
        _modeUpdateTimer = millis();
    }
}

void DisplayWorker::AddMode(ScreenMode* screenMode)
{
#ifdef _DEBUG
    DebugHelper::Log(this, "AddMode ");
#endif
    if (_modesCount + 1 > _allocatedCount)
    {
        ScreenMode** newModes = (ScreenMode**)realloc(screenModes, (sizeof(ScreenMode**) * (_modesCount + 1)));
        if (newModes != NULL)
        {
            _allocatedCount++;
            screenModes = newModes;
#ifdef _DEBUG
            DebugHelper::Log(this, "realloc ok!!!");
#endif
        }
        else
        {
#ifdef _DEBUG
            DebugHelper::Log(this, "realloc err!!!");
#endif
            return;
        }
    }

    screenModes[_modesCount] = screenMode;
    _modesCount++;

#ifdef _DEBUG
    DebugHelper::Log(this, "AddMode ok");
#endif
}

void DisplayWorker::OnConfigStart()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "config start");
#endif
    config->AddProperty(BACKLIGHT_ON_TIME_KEY, "Backlight on time (hh:mm)", "");
    config->AddProperty(BACKLIGHT_OFF_TIME_KEY, "Backlight off time (hh:mm)", "");
}

void DisplayWorker::OnConfigSave()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "config save");
#endif
}

void DisplayWorker::Setup()
{
    const char* turnOnTime = config->GetValue(BACKLIGHT_ON_TIME_KEY);
    const char* turnOffTime = config->GetValue(BACKLIGHT_OFF_TIME_KEY);
    if (turnOnTime == NULL || turnOffTime == NULL)
    {
        _backlightTime = NULL;
    }
    else
    {
#ifdef _DEBUG
        DebugHelper::Log(this, "Start parse bl time ");
#endif

        _backlightTime = (backlightTime*)malloc(sizeof(backlightTime));

        const char* substr;

        substr = strncpy((char*)malloc(sizeof(char) * 3), turnOnTime, 2);
        _backlightTime->startHour = atoi(substr);
        delete substr;
        _backlightTime->startMinute = atoi(turnOnTime + 3);

        substr = strncpy((char*)malloc(sizeof(char) * 3), turnOffTime, 2);
        _backlightTime->endHour = atoi(substr);
        delete substr;
        _backlightTime->endMinute = atoi(turnOffTime + 3);

#ifdef _DEBUG
        DebugHelper::Log(this, "Start bl time ");
        DebugHelper::Log(this, _backlightTime->startHour);
        DebugHelper::Log(this, ":");
        DebugHelper::Log(this, _backlightTime->endMinute);
        DebugHelper::Log(this, "End bl time ");
        DebugHelper::Log(this, _backlightTime->endHour);
        DebugHelper::Log(this, ":");
        DebugHelper::Log(this, _backlightTime->endMinute);
#endif
    }
    ShowMode(0);
    _changeModeTimer = millis();

    ShowMode(new SampleMode(lcd));
}

void DisplayWorker::Update()
{
    buttonStuff();

    if (_currMode->GetChangeTime() != -1 && millis() - _changeModeTimer > _currMode->GetChangeTime())
    {
        nextMode();
        _changeModeTimer = millis();
    }
    if (millis() - _modeUpdateTimer > _currMode->GetUpdateTime())
    {
        _currMode->Update();
        _modeUpdateTimer = millis();
    }

    if (millis() - _backlightUpdateTimer > 1000)
    {
        bool state = false;
        OVERRIDE_BACKLIGHT ob = _currMode->OverrideBacklight();
        switch (ob)
        {
        case OVERRIDE_BACKLIGHT::NO_OVERRIDE:
            if (timeStatus() == timeStatus_t::timeSet && _backlightTime != NULL)
            {
                int currHour = hour();
                int currMinute = minute();

                // #ifdef _DEBUG
                //                 DebugHelper::Log(this, "curr time ");
                //                 Serial.print(currHour);
                //                 Serial.print(":");
                //                 Serial.print(currMinute);
                //                 DebugHelper::Log(this, "Start bl time ");
                //                 Serial.print(_backlightTime->startHour);
                //                 Serial.print(":");
                //                 Serial.print(_backlightTime->endMinute);
                //                 DebugHelper::Log(this, "End bl time ");
                //                 Serial.print(_backlightTime->endHour);
                //                 Serial.print(":");
                //                 Serial.print(_backlightTime->endMinute);
                //                 Serial.println();
                // #endif
                if (currHour > _backlightTime->startHour || (currHour == _backlightTime->startHour && currMinute >= _backlightTime->startMinute))
                    if (currHour < _backlightTime->endHour || (currHour == _backlightTime->endHour && currMinute <= _backlightTime->endMinute))
                        state = true;
            }
            else
                state = true;
            break;
        case OVERRIDE_BACKLIGHT::OVERRIDE_ON:
            state = (true);
            break;

        case OVERRIDE_BACKLIGHT::OVERRIDE_OFF:
            state = (false);
            break;
        }
        lcd->setBacklight(state);
        _backlightUpdateTimer = millis();
    }
}

void DisplayWorker::nextMode()
{
    if (_currentModeIndex == -1)
        delete _currMode;

    _currentModeIndex++;
    if (_currentModeIndex >= _modesCount || _currentModeIndex < 0)
        _currentModeIndex = 0;

#ifdef _DEBUG
    DebugHelper::Log(this, "Next mode ");
    DebugHelper::Log(this, _currentModeIndex);
#endif

    ShowMode(_currentModeIndex);
}

void DisplayWorker::buttonStuff()
{
    if (millis() - _buttonCheckTimer > 25)
    {
        if (digitalRead(BUTTON_PIN))
        {
            if (!_button->buttonPressed)
            {
                _button->buttonPressTime = 0;
                _button->buttonPressed = true;
                _buttonCheckTimer = millis();
            }
            else
            {
                _button->buttonPressTime += millis() - _buttonCheckTimer;
                if (_button->buttonPressTime > 8000)
                {
                    _button->buttonPressTime = 100;
                }
            }
        }
        else
        {
            if (_button->buttonPressed)
            {
                _button->buttonPressed = false;

                if (_button->buttonPressTime > 1000)
                {
                    _button->longCount++;
                }
                else
                {
                    _button->clickCount++;
                }
                _sendButtonEventTimer = millis();
            }

            if (millis() - _sendButtonEventTimer > 250 && _button->buttonPressTime > 0)
            {
                buttonEvent(this->_button);
                _button->buttonPressTime = 0;
                _button->clickCount = 0;
                _button->longCount = 0;
                _sendButtonEventTimer = millis();
            }

        }
        _buttonCheckTimer = millis();
    }
}

void DisplayWorker::buttonEvent(button* arg)
{
#ifdef _DEBUG
    DebugHelper::Log(this, "onClick");
    DebugHelper::Log(this, "buttonPressTime");
    DebugHelper::Log(this, arg->buttonPressTime);
    DebugHelper::Log(this, "clickCount");
    DebugHelper::Log(this, arg->clickCount);
    DebugHelper::Log(this, "longCount");
    DebugHelper::Log(this, arg->longCount);
#endif
    // if (_currMode->MaySkipByButton())
    // {
    //     nextMode();
    // }
    _onbuttonEvent(arg);
}

void DisplayWorker::OnButtonEventCallback(onbuttonEvent callback)
{
    _onbuttonEvent = callback;
}

void DisplayWorker::initModes()
{
    //AddMode(new SampleMode(lcd));
    AddMode(new DateTimeMode(lcd));
}