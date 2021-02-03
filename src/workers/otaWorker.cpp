#include "workers/otaWorker.h"
#include "statics.h"

OTAWorker::OTAWorker(configHelper* ch, std::function<void()> startCallback, std::function<void()> endCallback, std::function<void(int progress)> progressCallback, std::function<void(ota_error_t error)> errorCallback)
{
    config = ch;
    _startCallback = startCallback;
    _endCallback = endCallback;
    _progressCallback = progressCallback;
    _errorCallback = errorCallback;
}

OTAWorker::~OTAWorker()
{

}

void OTAWorker::OnConfigStart()
{
    config->AddProperty("OTA Updates:");
    config->AddProperty(OTA_PASS_KEY, "OTA password", OTA_DEFAULT_PASS);
}

void OTAWorker::OnConfigSave()
{

}

void OTAWorker::Setup()
{
    ota = new ArduinoOTAClass();
    ota->setHostname(Statics::GetID());
    ota->setPassword(config->GetValueOrDefault(OTA_PASS_KEY, OTA_DEFAULT_PASS));

    ota->onStart(std::bind(&OTAWorker::OnStart, this));
    ota->onEnd(std::bind(&OTAWorker::OnEnd, this));
    ota->onError(std::bind(&OTAWorker::OnError, this, std::placeholders::_1));
    ota->onProgress(std::bind(&OTAWorker::OnProgress, this, std::placeholders::_1, std::placeholders::_2));
    ota->begin();
}

void OTAWorker::Update()
{
    ota->handle();
}

InternetNeedState OTAWorker::InternetNeed()
{
    return InternetNeedState::NEED_WIFI;
}

char* OTAWorker::Name()
{
    return "OTAWorker";
}

bool OTAWorker::IsUpdating()
{
    return _updating;
}

int OTAWorker::GetProgress()
{
    return _progress;
}

int* OTAWorker::GetProgressPtr()
{
    return &_progress;
}

void OTAWorker::OnStart()
{
    _updating = true;
    _startCallback();
}

void OTAWorker::OnEnd()
{
    _updating = false;
    _endCallback();
}

void OTAWorker::OnError(ota_error_t error)
{
    _errorCallback(error);
}

void OTAWorker::OnProgress(unsigned int progress, unsigned int total)
{
    _progress = progress / (total / 100);
    _progressCallback(_progress);
}