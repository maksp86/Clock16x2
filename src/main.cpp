
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
#include <ArduinoOTA.h>

#include "statics.h"
#include "helpers/debugHelper.h"
#include "helpers/configHelper.h"

#include "workers/worker.h"
#include "workers/wifiWorker.h"
#include "workers/mqttWorker.h"
#include "workers/ntpWorker.h"
#include "workers/displayWorker.h"
#include "workers/otaWorker.h"

#include "screen/msgMode.h"
#include "screen/progressbarMode.h"
#pragma endregion Includes

configHelper* confighelper;

WifiWorker* wifiworker;
MqttWorker* mqttworker;
NtpWorker* ntpWorker;
DisplayWorker* displayWorker;
OTAWorker* otaWorker;

WiFiManager WifiManager;
WiFiClient client;
PubSubClient mqttClient(client);

const int WORKERS_COUNT = 5;
Worker* workers[WORKERS_COUNT];

void configModeCallback(WiFiManager* manager)
{
#ifdef _DEBUG
  DebugHelper::Log("main", "config");
#endif
  displayWorker->ShowMode(new MsgMode(manager->getConfigPortalSSID().c_str(), "Config ap", (byte*)NULL, -1, false));

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
  displayWorker->ShowMode(new MsgMode("Config saved", true));
  confighelper->Save(); //must be first call here

  for (int i = 0; i < WORKERS_COUNT; i++)
  {
    workers[i]->OnConfigSave();
  }
}

void OnWifiConnected()
{
  displayWorker->ShowMode(new MsgMode(WiFi.SSID().c_str(), "Connected to", (WifiIcon), 5000, true));
  for (int i = 0; i < WORKERS_COUNT; i++)
  {
    if (workers[i]->InternetNeed() == InternetNeedState::NEED_WIFI)
      workers[i]->Setup();
    else if (workers[i]->InternetNeed() == InternetNeedState::NEED_INTERNET)
      if (wifiworker->HasInternet())
        workers[i]->Setup();
  }

  if (MDNS.begin(Statics::GetID()))
    Serial.println("MDNS started");
  else
    Serial.println("MDNS fail");
}

void OnMqttMessage(char* topic, uint8_t* payload, unsigned int payloadLength)
{

}

void OnButtonEvent(button* arg)
{
  if (arg->longCount == 1 && (arg->buttonPressTime > 5000 && arg->buttonPressTime < 7000))
  {
    displayWorker->ShowMode(new MsgMode("to start config", "Press 7 sec", (byte*)NULL, 2000, true));
  }
  else if (arg->buttonPressTime >= 7000)
  {
    WifiManager.startConfigPortal();
  }
}

void InitWorkers()
{
  wifiworker = new WifiWorker(&WifiManager, confighelper, OnWifiConnected);
  mqttworker = new MqttWorker(&mqttClient, confighelper, OnMqttMessage);
  ntpWorker = new NtpWorker(confighelper);
  displayWorker = new DisplayWorker(confighelper);
  displayWorker->OnButtonEventCallback(OnButtonEvent);

  otaWorker = new OTAWorker(confighelper,
    []()
    {
      displayWorker->ShowMode(new ProgressBarMode("UPDATING BY OTA", otaWorker->GetProgressPtr()));
    },
    []()
    {
      displayWorker->ShowMode(new MsgMode("COMPLETE", "OTA UPDATE", (byte*)NULL, 5000, true));
    },
      [](int progress)
    {
      displayWorker->Update();
    },
      [](ota_error_t err)
    {
      switch (err)
      {
      case ota_error_t::OTA_AUTH_ERROR:
        displayWorker->ShowMode(new MsgMode("OTA AUTH ERROR", true));
        break;
      case ota_error_t::OTA_BEGIN_ERROR:
        displayWorker->ShowMode(new MsgMode("OTA BEGIN ERROR", true));
        break;
      case ota_error_t::OTA_CONNECT_ERROR:
        displayWorker->ShowMode(new MsgMode("OTA CONNECT ERR", true));
        break;
      case ota_error_t::OTA_END_ERROR:
        displayWorker->ShowMode(new MsgMode("OTA END ERROR", true));
        break;
      case ota_error_t::OTA_RECEIVE_ERROR:
        displayWorker->ShowMode(new MsgMode("OTA RECEIVE ERR", true));
        break;
      }
    });

  workers[0] = displayWorker;
  workers[1] = wifiworker;
  workers[2] = mqttworker;
  workers[3] = ntpWorker;
  workers[4] = otaWorker;
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
  Serial.begin(74880, SERIAL_8N1, SERIAL_TX_ONLY);
  allowWork = !ifLastRebootIsError();
  if (!allowWork)
  {
    LiquidCrystal_I2C* lcd = new LiquidCrystal_I2C(I2C_ADDR, 16, 2, LCD_5x8DOTS);
    lcd->begin(PIN_SDA, PIN_SCL);
    lcd->backlight();
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
    if (otaWorker->IsUpdating())
    {
      displayWorker->Update();
      otaWorker->Update();
    }
    else
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

