
#pragma endregion Defines

#pragma region Includes
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>

#include "statics.h"
#include "helpers/debugHelper.h"
#include "helpers/configHelper.h"

#include "workers/worker.h"
#include "workers/wifiWorker.h"
#include "workers/mqttWorker.h"
#include "workers/ntpWorker.h"
#include "workers/displayWorker.h"
#pragma endregion Includes


configHelper* confighelper;

WifiWorker* wifiworker;
MqttWorker* mqttworker;
NtpWorker* ntpWorker;
DisplayWorker* displayWorker;

WiFiManager WifiManager;
WiFiClient client;
PubSubClient mqttClient(client);

const int WORKERS_COUNT = 4;
Worker* workers[WORKERS_COUNT];

void configModeCallback(WiFiManager* manager)
{
#ifdef _DEBUG
  DebugHelper::Log("main", "config");
#endif
  for (int i = 0; i < WORKERS_COUNT; i++)
  {
    workers[i]->OnConfigStart();
  }
}

void saveConfigCallback()
{
#ifdef _DEBUG
  DebugHelper::Log("main", "save");
#endif
  confighelper->Save(); //must be first call here

  for (int i = 0; i < WORKERS_COUNT; i++)
  {
    workers[i]->OnConfigSave();
  }
}

void OnWifiConnected()
{
  for (int i = 0; i < WORKERS_COUNT; i++)
  {
    if (workers[i]->InternetNeed() == InternetNeedState::NEED_WIFI)
      workers[i]->Setup();
    else if (workers[i]->InternetNeed() == InternetNeedState::NEED_INTERNET)
      if (wifiworker->HasInternet())
        workers[i]->Setup();
  }
}

void OnMqttMessage(char* topic, uint8_t* payload, unsigned int payloadLength)
{

}

void InitWorkers()
{
  wifiworker = new WifiWorker(&WifiManager, confighelper, OnWifiConnected);
  mqttworker = new MqttWorker(&mqttClient, confighelper, OnMqttMessage);
  ntpWorker = new NtpWorker(confighelper);
  displayWorker = new DisplayWorker(confighelper);
  workers[0] = displayWorker;
  workers[1] = wifiworker;
  workers[2] = mqttworker;
  workers[3] = ntpWorker;
}

bool ifLastRebootIsError()
{
  rst_info* rst = ESP.getResetInfoPtr();
  if (rst->reason == rst_reason::REASON_EXCEPTION_RST)
  {
    Serial.println("REBOOT WITH ERROR!!!!");
    return true;
  }
  else return false;
}

bool allowWork;

void setup()
{
  Serial.begin(74880);
  allowWork = !ifLastRebootIsError();
  if (!allowWork)
  {
    LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(I2C_ADDR, 16, 2, LCD_5x8DOTS);
    lcd->begin(PIN_SDA, PIN_SCL);
    lcd->print(F("REBOOT WITH ERR!"));
    lcd->setCursor(0, 1);
    lcd->print(F("NEED DEBUG"));
    lcd->flush();
    return;
  }

#ifdef _DEBUG
  DebugHelper::Log("main", "Boot");
  DebugHelper::Log("main", Statics::GetID());
#endif

  confighelper = new configHelper(&WifiManager);
  confighelper->Load();

  WifiManager.setAPCallback(configModeCallback);
  WifiManager.setSaveConfigCallback(saveConfigCallback);

  InitWorkers();
  for (int i = 0; i < WORKERS_COUNT; i++)
  {
    if (workers[i]->InternetNeed() == NO_NEED)
      workers[i]->Setup();
  }
  yield();
}

void loop()
{
  if (allowWork)
    for (int i = 0; i < WORKERS_COUNT; i++)
    {
      if (workers[i]->InternetNeed() > 0)
      {
        if (!wifiworker->IsConnected())
        {
          continue;
        }

        if (workers[i]->InternetNeed() == InternetNeedState::NEED_WIFI)
          workers[i]->Update();
        else if (workers[i]->InternetNeed() == InternetNeedState::NEED_INTERNET)
          if (wifiworker->HasInternet())
            workers[i]->Update();
      }
      else
        workers[i]->Update();
    }
  yield();
}

