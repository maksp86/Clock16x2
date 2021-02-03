#ifndef WORKERS_SCREENWORKER_H
#define WORKERS_SCREENWORKER_H

#include "worker.h"
#include "helpers/configHelper.h"
#include "screen/screenMode.h"
#include <LiquidCrystal_I2C.h>

struct backlightTime
{
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
};

struct button
{
    /* data */
    long buttonPressTime = 0;
    bool buttonPressed = false;
    int clickCount = 0;
    int longCount = 0;
};


class DisplayWorker : public Worker
{
    typedef std::function<void(button* arg)> onbuttonEvent;

private:
    LiquidCrystal_I2C* lcd;

    ~DisplayWorker();
    void nextMode();
    void buttonStuff();

    void buttonEvent(button* arg);
    void initModes();

    configHelper* config;

    ScreenMode** screenModes;
    ScreenMode* _currMode;

    backlightTime* _backlightTime;

    button* _button;

    bool _mayWork;

    int _currentModeIndex;
    int _modesCount;
    int _allocatedCount;

    uint32_t _backlightUpdateTimer;
    uint32_t _modeUpdateTimer;
    uint32_t _changeModeTimer;

    uint32_t _buttonCheckTimer;
    uint32_t _sendButtonEventTimer;

    onbuttonEvent _onbuttonEvent;
public:
    DisplayWorker();
    DisplayWorker(configHelper* ch);

    void ShowMode(int index);
    void ShowMode(ScreenMode* customMode);

    void OnButtonEventCallback(onbuttonEvent callback);

    void AddMode(ScreenMode* screenMode);
    void OnConfigStart() override;
    void OnConfigSave() override;
    void Setup() override;
    void Update() override;
    InternetNeedState InternetNeed() override;
    char* Name() override;
};

#endif