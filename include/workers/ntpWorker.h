#ifndef WORKERS_NTPWORKER_H
#define WORKERS_NTPWORKER_H

#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <WiFiUdp.h>

#include "worker.h"
#include "helpers/configHelper.h"

class NtpWorker : public Worker
{
private:
    ~NtpWorker();
    WiFiUDP udp;
    configHelper* config;
    time_t getNtpTime();
    void sendNTPpacket(IPAddress& address);
    byte* packetBuffer;

    bool _enabled;
    uint32_t _nextSyncTimer;
    unsigned int _syncTime = 300000;
public:
    NtpWorker();
    NtpWorker(configHelper* ch);

    void OnConfigStart() override;
    void OnConfigSave() override;
    void Setup() override;
    void Update() override;
    InternetNeedState InternetNeed() override;
    char* Name() override;
};

#endif