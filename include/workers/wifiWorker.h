#ifndef WORKERS_WIFIWORKER_H_INCLUDED
#define WORKERS_WIFIWORKER_H_INCLUDED

#include "worker.h"

#include <list>
#include <functional>
#define CONNECTED_CALLBACK_SIGN std::function<void()> connectedCallback
#include <WiFiManager.h>
#include "helpers/configHelper.h"

class WifiWorker : public Worker
{
private:
    CONNECTED_CALLBACK_SIGN;
    ~WifiWorker();
    WiFiManager* manager;
    configHelper* confighelper;

    bool _hasInternet;
    uint32_t _pingTimer;
    int _pingTime = 15000;

    bool _connected;
    uint32_t _connectedCheckTimer;
    int _connectedCheckTime = 500;

    WiFiEventHandler ConnectedHandler;
    WiFiEventHandler DisconnectedHandler;
    WiFiEventHandler GotIpHandler;

    void checkInternet();
public:
    WifiWorker();
    WifiWorker(WiFiManager* man, configHelper* ch, CONNECTED_CALLBACK_SIGN);
    void SetCallbacks(CONNECTED_CALLBACK_SIGN);
    void Setup() override;
    void Update() override;
    bool IsConnected();
    bool HasInternet();
    void OnConfigSave() override;
    void OnConfigStart() override;
    InternetNeedState InternetNeed() override;
    char* Name() override;
    void onStationConnected(const WiFiEventStationModeConnected& evt);
    void onStationDisconnected(const WiFiEventStationModeDisconnected& evt);
    void onStationGotIP(const WiFiEventStationModeGotIP& e);
};

#endif