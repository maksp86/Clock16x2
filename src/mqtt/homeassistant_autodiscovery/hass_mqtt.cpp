#include "hass_mqtt.h"

#ifdef USE_HOMEASSISTANT

const char* config_format_template PROGMEM = "{\"~\":\"%s\",\"name\":\"%S\",\"uniq_id\":\"%s\",\"avty_t\":\"~state\",%S\"dev\":{\"ids\":\"%s\",\"name\":\"%s\",\"sw\":\"%s\",\"mf\":\"%s\"}}";
const char* sensor_config_topic_template PROGMEM = "homeassistant/%s/%s%s/config";

void autodiscovery_send(AsyncMqttClient* client)
{
    char* uniq_id = new char[strlen(get_device_name()) + 15];
    char* out = new char[300];
    char* topic = new char[strlen(get_device_name()) + 40];


    sprintf(uniq_id, "%s-lockmode", get_device_name());

    size_t out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("Lock mode"), uniq_id, PSTR("\"cmd_t\":\"~lock/set\",\"opt\":true,\"ic\":\"mdi:lock-outline\","),
        get_device_name(), DEVICE_NAME_PREFIX, SW_VER, SW_MAINTAINER);

    sprintf_P(topic, sensor_config_topic_template, "switch", get_device_name(), "l");

    client->publish(topic, 0, true, out, out_len);


    sprintf(uniq_id, "%s-wifirssi", get_device_name());

    out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("WIFI Strength"), uniq_id, PSTR("\"stat_t\":\"~rssi\",\"unit_of_meas\":\"%\",\"dev_cla\":\"signal_strength\","),
        get_device_name(), DEVICE_NAME_PREFIX, SW_VER, SW_MAINTAINER);

    sprintf_P(topic, sensor_config_topic_template, "sensor", get_device_name(), "w");

    client->publish(topic, 0, true, out, out_len);


    sprintf(uniq_id, "%s-uptime", get_device_name());

    out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("Uptime"), uniq_id, PSTR("\"stat_t\":\"~uptime\",\"unit_of_meas\":\"s\",\"ic\":\"mdi:timer\","),
        get_device_name(), DEVICE_NAME_PREFIX, SW_VER, SW_MAINTAINER);

    sprintf_P(topic, sensor_config_topic_template, "sensor", get_device_name(), "u");

    client->publish(topic, 0, true, out, out_len);

    delete[] uniq_id;
    delete[] out;
    delete[] topic;

    send_sensors(true);
    // strcpy(name, "Lock mode");


    // StaticJsonDocument<192> doc;
    // doc["~"] = mqtt_topic_start();
    // doc["name"] = (const char*)name;
    // doc["unique_id"] = (const char*)uniq_id;
    // doc["cmd_t"] = "~lock/set";
    // doc["stat_t"] = "~state";
    // doc["ic"] = "mdi:lock-outline";
    // doc["opt"] = true;

    // JsonObject dev = doc.createNestedObject("dev");
    // dev["name"] = DEVICE_NAME_PREFIX;
    // dev["ids"] = get_device_name();
    // dev["sw"] = SW_VER;
    // dev["mf"] = SW_MAINTAINER;
}

uint32_t mqtt_sensors_send_timer = 0;

void send_sensors(bool force)
{
    if (millis() - mqtt_sensors_send_timer > 30000 || force)
    {
        if (mqtt_is_connected())
        {
            char* payload = new char[15];
            char* topic = new char[strlen(mqtt_topic_start()) + 10];

            sprintf(topic, "%s%s", mqtt_topic_start(), "rssi");
            sprintf(payload, "%u", get_percent_from_RSSI(WiFi.RSSI()));

            mqtt_get_client()->publish(topic, 0, false, payload);

            sprintf(topic, "%s%s", mqtt_topic_start(), "uptime");
            sprintf(payload, "%lu", millis() / 1000);

            mqtt_get_client()->publish(topic, 0, false, payload);

            delete[] payload;
            delete[] topic;
        }
        mqtt_sensors_send_timer = millis();
    }
}
#endif