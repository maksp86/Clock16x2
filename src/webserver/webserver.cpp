#include "webserver.h"

#ifdef USE_WEBSERVER

#include "routes/upload_route.h"
#include "routes/message_route.h"

DNSServer* dns;
AsyncWebServer* server;

const char* APPLICATION_JSON_LITERAL = "application/json";

/*
%DEV_NAME%
%STATUSJSON% //index
%CONFNAME%
*/
String commonProcessor(const String& var)
{
    if (var == F("DEV_NAME"))
        return get_device_name();
    else if (var == F("CONFNAME"))
        return F(CONFIG_FILENAME);
    return String();
}

void webserver_setup()
{
    server = new AsyncWebServer(80);
    dns = new DNSServer();

    server->on("/getconfig", [](AsyncWebServerRequest* request) {
        config_unload();
        request->send(LittleFS, CONFIG_FILENAME, APPLICATION_JSON_LITERAL);
        });

    server->on("/getstatus", [](AsyncWebServerRequest* request) {
        AsyncResponseStream* response = request->beginResponseStream(APPLICATION_JSON_LITERAL);

        response->printf_P(PSTR("{\"WiFi\":{\"Status\":\"%s\",\"RSSI\":\"%ddBm\"}"), wifi_get_last_status() == wifi_status::WIFI_CONNECTED ? "STA" : "AP", WiFi.RSSI());

        response->printf_P(PSTR(",\"MQTT\":{\"Status\":\"%sconnected\"}"), mqtt_is_connected() ? "" : "dis");
        response->printf_P(PSTR(",\"NTP\":{\"Time got\":\"%d\",\"Timestamp\":\"%llu\"}"), ntp_time_set(), *ntp_get_epoch_time());

#ifdef HTU21D_ENABLED
        response->printf_P(PSTR(",\"HTU21D\":{\"Status\":\"%sconnected\",\"Temperature\":\"%.2f&deg;C\",\"Humidity\":\"%.2f%%\"}"),
            htu21d_is_connected() ? "" : "dis", htu21d_get_temperature(), htu21d_get_humidity());
#endif
        response->printf_P(PSTR(",\"Device\":{\"Free heap\":\"%uB\",\"Uptime\":\"%us\",\"Reboot reason\":\"%s\"}"), ESP.getFreeHeap(), millis() / 1000, ESP.getResetReason().c_str());
        response->print("}");
        request->send(response);
        });

    server->on("/reboot", [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", PSTR("<meta http-equiv=\"refresh\" content=\"5;url=/\"/>Device is now rebooting"));
        set_need_restart();
        });

    server->onRequestBody(handleBody);

    server->on("/upload", HTTP_POST, [](AsyncWebServerRequest* request) { request->send(200); }, handleUpload);

    server->serveStatic("/style.css", LittleFS, "/style.css");

    server->serveStatic("/", LittleFS, "/").setTemplateProcessor(commonProcessor);
}

void webserver_loop()
{
    if (wifi_get_last_status() == wifi_status::WIFI_AP_ENABLED)
    {
        dns->processNextRequest();
    }
}

void webserver_wifi_on_event(wifi_event event)
{
    switch (event)
    {
    case wifi_event::WIFI_ON_CONNECTED:
        server->begin();
        break;
    case wifi_event::WIFI_ON_DISCONNECT:
        server->end();
        break;

    case wifi_event::WIFI_ON_AP_START:
#if DEBUG >= 1
        Serial.printf("dns started on %s", WiFi.softAPIP().toString().c_str());
#endif
        dns->start(53, "*", WiFi.softAPIP());
        server->begin();
        break;

    case wifi_event::WIFI_ON_AP_END:
        dns->stop();
        server->end();
        break;
    }
}
#endif