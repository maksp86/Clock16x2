#include "hass_mqtt.h"

#ifdef USE_HOMEASSISTANT

const char* config_format_template PROGMEM = "{\"~\":\"%s\",\"name\":\"%S\",\"uniq_id\":\"%s\",\"avty_t\":\"~state\",%S\"dev\":{\"ids\":\"%s\",\"name\":\"%s\",\"sw\":\"%s\",\"mf\":\"%s\",\"cu\":\"http://%s\"}}";
const char* sensor_config_topic_template PROGMEM = "homeassistant/%s/%s%s/config";

void autodiscovery_send(AsyncMqttClient* client)
{
    char* uniq_id = new char[strlen(get_device_name()) + 15];
    char* out = new char[350];
    char* topic = new char[strlen(get_device_name()) + 40];

    char* dev_prefix = nullptr;

    const char* dev_prefix_ptr = config_get_str("dev_prefix", &dev_prefix) ? dev_prefix : PSTR("espclock");

    sprintf(uniq_id, "%s-lockmode", get_device_name());

    size_t out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("Lock mode"), uniq_id, PSTR("\"cmd_t\":\"~lock/set\",\"opt\":true,\"ic\":\"mdi:lock-outline\","),
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER, WiFi.localIP().toString().c_str());

    sprintf_P(topic, sensor_config_topic_template, "switch", get_device_name(), "l");

    client->publish(topic, 0, true, out, out_len);


    sprintf(uniq_id, "%s-wifirssi", get_device_name());

    out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("WIFI Strength"), uniq_id, PSTR("\"stat_t\":\"~rssi\",\"unit_of_meas\":\"dBm\",\"dev_cla\":\"signal_strength\","),
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER, WiFi.localIP().toString().c_str());

    sprintf_P(topic, sensor_config_topic_template, "sensor", get_device_name(), "w");

    client->publish(topic, 0, true, out, out_len);


    sprintf(uniq_id, "%s-uptime", get_device_name());

    out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("Uptime"), uniq_id, PSTR("\"stat_t\":\"~uptime\",\"unit_of_meas\":\"s\",\"ic\":\"mdi:timer\",\"dev_cla\":\"duration\","),
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER, WiFi.localIP().toString().c_str());

    sprintf_P(topic, sensor_config_topic_template, "sensor", get_device_name(), "u");

    client->publish(topic, 0, true, out, out_len);

#ifdef HTU21D_ENABLED
    sprintf(uniq_id, "%s-temp", get_device_name());

    out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("Temperature"), uniq_id, PSTR("\"stat_t\":\"~temp\",\"dev_cla\":\"temperature\",\"unit_of_meas\":\"°C\","),
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER, WiFi.localIP().toString().c_str());

    sprintf_P(topic, sensor_config_topic_template, "sensor", get_device_name(), "t");

    client->publish(topic, 0, true, out, out_len);


    sprintf(uniq_id, "%s-hum", get_device_name());

    out_len = sprintf_P(out, config_format_template,
        mqtt_topic_start(), PSTR("Humidity"), uniq_id, PSTR("\"stat_t\":\"~hum\",\"dev_cla\":\"humidity\",\"unit_of_meas\":\"%\","),
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER, WiFi.localIP().toString().c_str());

    sprintf_P(topic, sensor_config_topic_template, "sensor", get_device_name(), "h");

    client->publish(topic, 0, true, out, out_len);
#endif

    delete[] uniq_id;
    delete[] out;
    delete[] topic;

    if (dev_prefix)
        delete[] dev_prefix;

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
    // dev["name"] = dev_prefix_ptr;
    // dev["ids"] = get_device_name();
    // dev["sw"] = SW_VER;
    // dev["mf"] = SW_MAINTAINER;
}

uint32_t mqtt_sensors_send_timer = 0;

void send_sensors(bool force)
{
    if (millis() - mqtt_sensors_send_timer > 60000 || force)
    {
        if (mqtt_is_connected())
        {
            char* payload = new char[15];
            char* topic = new char[strlen(mqtt_topic_start()) + 10];

            sprintf(topic, "%s%s", mqtt_topic_start(), "rssi");
            sprintf(payload, "%d", WiFi.RSSI());

            mqtt_get_client()->publish(topic, 0, false, payload);


            sprintf(topic, "%s%s", mqtt_topic_start(), "uptime");
            sprintf(payload, "%lu", millis() / 1000);

            mqtt_get_client()->publish(topic, 0, false, payload);

#ifdef HTU21D_ENABLED
            sprintf(topic, "%s%s", mqtt_topic_start(), "hum");
            sprintf(payload, "%.2f", htu21d_get_humidity());

            mqtt_get_client()->publish(topic, 0, false, payload);


            sprintf(topic, "%s%s", mqtt_topic_start(), "temp");
            sprintf(payload, "%.2f", htu21d_get_temperature());

            mqtt_get_client()->publish(topic, 0, false, payload);
#endif

            delete[] payload;
            delete[] topic;
        }
        mqtt_sensors_send_timer = millis();
    }
}
#endif