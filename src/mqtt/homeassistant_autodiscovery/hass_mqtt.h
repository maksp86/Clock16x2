#ifndef __HASS_MQTT_H__
#define __HASS_MQTT_H__

#include "defines.h"
#ifdef USE_HOMEASSISTANT

#include "ArduinoJson.h"

#include "helpers.h"
#include "mqtt/mqtt.h"
#include "htu21d/htu21d.h"
#include "config/config.h"

void autodiscovery_send(AsyncMqttClient* client);
void send_sensors(bool force = false);

#endif

#endif // __HASS_MQTT_H__