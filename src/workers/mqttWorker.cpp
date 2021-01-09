#include "workers/mqttWorker.h"

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

#include "helpers/debugHelper.h"
#include "helpers/configHelper.h"
#include "statics.h"

static char* MQTT_SERVER_KEY = "mqtt_server";
static char* MQTT_PORT_KEY = "mqtt_port";
static char* MQTT_USER_KEY = "mqtt_user";
static char* MQTT_PASS_KEY = "mqtt_pass";

MqttWorker::MqttWorker()
{
    reconnectTimer = 0;
    reconnectTime = 5000;
#ifdef _DEBUG
    DebugHelper::Log(this, "Init");
#endif
}

MqttWorker::MqttWorker(PubSubClient* mc, configHelper* ch, std::function<void(char*, uint8_t*, unsigned int)> callback) : MqttWorker()
{
    mqtt = mc;
    config = ch;

    _callback = callback;
    mqtt->setCallback(std::bind(&MqttWorker::MqttCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

MqttWorker::~MqttWorker()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "destruct");
#endif
}

void MqttWorker::Setup()
{
    const char* serverIP = config->GetValue(MQTT_SERVER_KEY);
    const char* serverPort = config->GetValue(MQTT_PORT_KEY);
    if (serverIP == NULL || serverPort == NULL)
        MayWork = false;
    else
    {
        MayWork = true;
#ifdef _DEBUG
        DebugHelper::Log(this, "mqtt server");
        DebugHelper::Log(this, serverIP);
        DebugHelper::Log(this, serverPort);
#endif  
        mqtt->setServer(serverIP, atoi(serverPort));
        mqtt->setBufferSize(512);
        mqtt->setSocketTimeout(2);
        MqttReconnect();
    }
}

void MqttWorker::Update()
{
    if (!MayWork) return;
    if (mqtt->connected())
        mqtt->loop();
    else if (millis() - reconnectTimer > reconnectTime)
    {
        MqttReconnect();
        reconnectTimer = millis();
    }
}

void MqttWorker::MqttReconnect()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "mqtt connecting...");
#endif  
    if (mqtt->connected())
        mqtt->disconnect();

    const char* userParam = config->GetValue(MQTT_USER_KEY);
    const char* passParam = config->GetValue(MQTT_PASS_KEY);
    if (userParam == NULL || passParam == NULL)
    {
        DebugHelper::Log(this, "mqtt creds null!!!");
        return;
    }
#ifdef _DEBUG
    DebugHelper::Log(this, "mqtt creds");
    DebugHelper::Log(this, userParam);
    DebugHelper::Log(this, passParam);
#endif  
    if (mqtt->connect(Statics::GetID(), userParam, passParam))
    {
#ifdef _DEBUG
        DebugHelper::Log(this, "mqtt connected!");
#endif  
        connectFails = 0;
        reconnectTime = 5000;
        MqttSubscribe();
    }
    else
    {
#ifdef _DEBUG
        DebugHelper::Log(this, "mqtt connection failed");
#endif       
        connectFails += 1;
        if (connectFails > 3)
            reconnectTime = 20000;
    }
}

void MqttWorker::MqttSubscribe()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "sub start");
#endif
}

void MqttWorker::MqttCallback(char* topic, byte* payload, unsigned int length)
{
#ifdef _DEBUG
    DebugHelper::Log(this, "New message from topic: ");
    DebugHelper::Log(this, topic);
#endif
    this->_callback(topic, payload, length);
}

void MqttWorker::OnConfigStart()
{
#ifdef _DEBUG
    DebugHelper::Log(this, "config start");
#endif
    config->AddProperty("<br>MQTT configuration:");
    config->AddProperty(MQTT_SERVER_KEY, "mqtt server", "");
    config->AddProperty(MQTT_PORT_KEY, "mqtt port", "1883");
    config->AddProperty(MQTT_USER_KEY, "mqtt user", "");
    config->AddProperty(MQTT_PASS_KEY, "mqtt password", "");
#ifdef _DEBUG
    DebugHelper::Log(this, "config end");
#endif
}

void MqttWorker::OnConfigSave()
{
}

InternetNeedState MqttWorker::InternetNeed()
{
    return InternetNeedState::NEED_WIFI;
}

char* MqttWorker::Name()
{
    return "MqttWorker";
}
