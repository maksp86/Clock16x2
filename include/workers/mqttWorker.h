#ifndef WORKERS_MQTTHELPER_H
#define WORKERS_MQTTHELPER_H

#include <ESP8266WiFi.h>
#include "PubSubClient.h"

#include "worker.h"
#include "helpers/configHelper.h"

class MqttWorker : public Worker
{
private:
    ~MqttWorker();
    PubSubClient* mqtt;
    WiFiClient* client;
    configHelper* config;

    uint32_t reconnectTimer;
    int reconnectTime;
    int connectFails;

    bool MayWork;

    std::function<void(char*, uint8_t*, unsigned int)> _callback;

    void MqttSubscribe();
    void MqttReconnect();

public:
    MqttWorker();
    MqttWorker(PubSubClient* mc, configHelper* ch, std::function<void(char*, uint8_t*, unsigned int)> callback);

    void OnConfigStart() override;
    void OnConfigSave() override;
    void MqttCallback(char* topic, byte* payload, unsigned int length);
    void Setup() override;
    void Update() override;
    InternetNeedState InternetNeed() override;
    char* Name() override;
};

#endif