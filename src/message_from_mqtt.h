#ifndef __MESSAGE_FROM_MQTT_H__
#define __MESSAGE_FROM_MQTT_H__

#include <ArduinoJson.h>

#include "display/display.h"
#include "display/modes/message_mode/message_mode.h"
#include "mqtt/mqtt.h"

void check_message(const char* messagejson, size_t len);
void on_message_mqtt(char* topic, char* payload, size_t len);

void subscribe_to_message(AsyncMqttClient* client);
#endif // __MESSAGE_FROM_MQTT_H__