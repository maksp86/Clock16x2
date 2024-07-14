#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include "defines.h"
#ifdef USE_WEBSERVER

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

#include "helpers.h"
#include "wifi/wifi.h"
#include "config/config.h"

void webserver_setup();
void webserver_loop();
void webserver_wifi_on_event(wifi_event event);

#endif
#endif // __WEBSERVER_H__