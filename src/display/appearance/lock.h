#ifndef __LOCK_H__
#define __LOCK_H__

#include <Arduino.h>
#include <AsyncMqttClient.h>

#include "mqtt/mqtt.h"

void on_lock_message(char* topic, char* payload, size_t len);
void subscribe_to_lock(AsyncMqttClient* client);
bool get_lock_state();

#endif // __LOCK_H__