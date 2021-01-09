#include "workers/wifiworker.h"

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266Ping.h>

#include "statics.h"
#include "helpers/debugHelper.h"
#include "helpers/eepromHelper.h"

WifiWorker::WifiWorker()
{
#ifdef _DEBUG
  DebugHelper::Log(this, "Init");
#endif
  WiFi.mode(WIFI_STA);
}

WifiWorker::WifiWorker(WiFiManager* man, configHelper* ch, CONNECTED_CALLBACK_SIGN) : WifiWorker()
{
  confighelper = ch;
  manager = man;
  manager->setWiFiAutoReconnect(true);

  this->connectedCallback = connectedCallback;
  WiFi.onStationModeConnected(std::bind(&WifiWorker::onStationConnected, this, std::placeholders::_1));
  WiFi.onStationModeDisconnected(std::bind(&WifiWorker::onStationDisconnected, this, std::placeholders::_1));
  WiFi.onStationModeGotIP(std::bind(&WifiWorker::onStationGotIP, this, std::placeholders::_1));
}

WifiWorker::~WifiWorker()
{
#ifdef _DEBUG
  DebugHelper::Log(this, "destruct");
#endif
}

void WifiWorker::Setup()
{
#ifdef _DEBUG
  DebugHelper::Log(this, "Setup");
#endif
  //manager->startConfigPortal();
  manager->setConnectTimeout(180);
  if (manager->autoConnect())
  {
    _connected = true;
    checkInternet();
    this->connectedCallback();
  }
}

void WifiWorker::checkInternet()
{
  if (!IsConnected())
  {
    _hasInternet = false;
    return;
  }
  _hasInternet = Ping.ping(IPAddress(8, 8, 8, 8), 1);

  if (_hasInternet)
  {
    _pingTime = 120000;
  }
  else
  {
#ifdef _DEBUG
    DebugHelper::Log(this, "ping fail");
#endif
    _pingTime = 30000;
  }
}

void WifiWorker::Update()
{
  if (millis() - _pingTimer > _pingTime)
  {
    checkInternet();
    yield();
    _pingTimer = millis();
  }

  // if (millis() - _connectedCheckTimer > _connectedCheckTime)
  // {
  //   WiFi.isConnected();
  //   _connectedCheckTimer = millis();
  // }
}

bool WifiWorker::IsConnected()
{
  return _connected;
}

bool WifiWorker::HasInternet()
{
  return _hasInternet;
}

void WifiWorker::SetCallbacks(CONNECTED_CALLBACK_SIGN)
{
  this->connectedCallback = connectedCallback;
}

void WifiWorker::OnConfigSave()
{
#ifdef _DEBUG
  DebugHelper::Log(this, "config save");
#endif
}

void WifiWorker::OnConfigStart()
{
#ifdef _DEBUG
  DebugHelper::Log(this, "config start");
#endif
  confighelper->AddProperty(Statics::GetID());
#ifdef _DEBUG
  DebugHelper::Log(this, "config end");
#endif
}

InternetNeedState WifiWorker::InternetNeed()
{
  return InternetNeedState::NO_NEED;
}

char* WifiWorker::Name()
{
  return "WifiWorker";
}

void WifiWorker::onStationConnected(const WiFiEventStationModeConnected& evt) {
#ifdef _DEBUG
  DebugHelper::Log(this, "Station connected");
  DebugHelper::Log(this, evt.ssid.c_str());
#endif
}

void WifiWorker::onStationDisconnected(const WiFiEventStationModeDisconnected& evt) {
#ifdef _DEBUG
  DebugHelper::Log(this, "Station disconnected");
  DebugHelper::Log(this, evt.ssid.c_str());
#endif
  _connected = false;
}

void WifiWorker::onStationGotIP(const WiFiEventStationModeGotIP& e)
{
#ifdef _DEBUG
  DebugHelper::Log(this, "Got ip ");
  e.ip.printTo(Serial);
#endif
  if (!_connected)
  {
    _connected = true;
    checkInternet();
    this->connectedCallback();
  }
}