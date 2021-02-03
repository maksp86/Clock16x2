#ifndef __OTAWORKER_H__
#define __OTAWORKER_H__

#define NO_GLOBAL_ARDUINOOTA
#include <ArduinoOTA.h>

#include "workers/worker.h"
#include "helpers/configHelper.h"

class OTAWorker : public Worker
{
private:
    int _progress = 0;
    bool _updating = false;
    configHelper* config;
    ArduinoOTAClass* ota;

    std::function<void()> _startCallback;
    std::function<void()> _endCallback;
    std::function<void(ota_error_t error)> _errorCallback;
    std::function<void(int progress)> _progressCallback;

    void OnStart();
    void OnEnd();
    void OnError(ota_error_t error);
    void OnProgress(unsigned int progress, unsigned int total);
public:
    OTAWorker(configHelper* ch, std::function<void()> startCallback, std::function<void()> endCallback, std::function<void(int progress)> progressCallback, std::function<void(ota_error_t error)> errorCallback);
    ~OTAWorker();

    bool IsUpdating();
    int GetProgress();
    int* GetProgressPtr();

    void OnConfigStart() override;
    void OnConfigSave() override;
    void Setup() override;
    void Update() override;
    InternetNeedState InternetNeed() override;
    char* Name() override;
};

#define OTA_DEFAULT_PASS "12345678"
#define OTA_PASS_KEY "otapass"

#endif // __OTAWORKER_H__