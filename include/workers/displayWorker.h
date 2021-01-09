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
    long buttonPressTime;
    uint32_t buttonCheckTimer;
    bool buttonPressed;
};



class DisplayWorker : public Worker
{
private:
    LiquidCrystal_I2C* lcd;

    ~DisplayWorker();
    void nextMode();
    void buttonStuff();
    void onClick();
    void onLongPress();
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
public:
    DisplayWorker();
    DisplayWorker(configHelper* ch);

    void ShowMode(int index);
    void ShowMode(ScreenMode* customMode);

    void AddMode(ScreenMode* screenMode);
    void OnConfigStart() override;
    void OnConfigSave() override;
    void Setup() override;
    void Update() override;
    InternetNeedState InternetNeed() override;
    char* Name() override;
};

#endif