#ifndef __MQTT_H__
#define __MQTT_H__

#include <Arduino.h>
#include <AsyncMqttClient.h>

#include "display/appearance/statusbar.h"
#include "wifi/wifi.h"
#include "config/config.h"
#include "helpers.h"

void mqtt_setup();
void mqtt_update();
void mqtt_connect();
// void mqtt_connect(IPAddress server, uint16_t port, const char* login = nullptr, const char* password = nullptr);
// void mqtt_connect(const char* server, uint16_t port, const char* login = nullptr, const char* password = nullptr);
void mqtt_disconnect(bool force);
bool mqtt_is_connected();
AsyncMqttClient* mqtt_get_client();
void mqtt_on_connected(AsyncMqttClient* client);
void mqtt_on_message(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
const char* mqtt_topic_start();

#endif // __MQTT_H__