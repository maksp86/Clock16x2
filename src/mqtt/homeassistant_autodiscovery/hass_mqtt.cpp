#include "hass_mqtt.h"

#ifdef USE_HOMEASSISTANT

#ifdef USE_WEBSERVER
const char* config_format_template PROGMEM = "{\"~\":\"%s\",\"name\":\"%S\",\"uniq_id\":\"%s\",\"avty_t\":\"~state\",%S\"dev\":{\"ids\":\"%s\",\"name\":\"%s\",\"sw\":\"%s\",\"mf\":\"%s\",\"cu\":\"http://%s\"}}";
#else
const char* config_format_template PROGMEM = "{\"~\":\"%s\",\"name\":\"%S\",\"uniq_id\":\"%s\",\"avty_t\":\"~state\",%S\"dev\":{\"ids\":\"%s\",\"name\":\"%s\",\"sw\":\"%s\",\"mf\":\"%s\"}}";
#endif
const char* sensor_config_topic_template PROGMEM = "homeassistant/%s/%s%s/config";

size_t autodiscovery_sprintf_config(char* out, const char* uniq_id, const char* dev_prefix_ptr, const char* sensor_name, const char* sensor_definition)
{
#ifdef USE_WEBSERVER
    return sprintf_P(out, config_format_template,
        mqtt_topic_start(), sensor_name, uniq_id, sensor_definition,
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER, WiFi.localIP().toString().c_str());
#else
    return sprintf_P(out, config_format_template,
        mqtt_topic_start(), sensor_name, uniq_id, sensor_definition,
        get_device_name(), dev_prefix_ptr, SW_VER, SW_MAINTAINER);
#endif
}

const char* SENSOR_LITERAL = "sensor";

void autodiscovery_send(AsyncMqttClient* client)
{
    size_t dev_name_len = strlen(get_device_name());
    char* sensor_uniq_id = new char[dev_name_len + 15];
    char* topic = new char[dev_name_len + 40];

    char* mqtt_out_buf = new char[350];

    char* dev_prefix = nullptr;
    const char* dev_prefix_ptr = config_get_str("dev_prefix", &dev_prefix) ? dev_prefix : PSTR("espclock");


    sprintf_P(sensor_uniq_id, PSTR("%s-lockmode"), get_device_name());
    size_t out_len = autodiscovery_sprintf_config(mqtt_out_buf, sensor_uniq_id, dev_prefix_ptr, PSTR("Lock mode"), PSTR("\"cmd_t\":\"~lock/set\",\"opt\":true,\"ic\":\"mdi:lock-outline\","));

    sprintf_P(topic, sensor_config_topic_template, "switch", get_device_name(), "l");
    client->publish(topic, 0, true, mqtt_out_buf, out_len);


    sprintf_P(sensor_uniq_id, PSTR("%s-wifirssi"), get_device_name());
    out_len = autodiscovery_sprintf_config(mqtt_out_buf, sensor_uniq_id, dev_prefix_ptr, PSTR("WIFI Strength"), PSTR("\"stat_t\":\"~rssi\",\"unit_of_meas\":\"dBm\",\"dev_cla\":\"signal_strength\","));

    sprintf_P(topic, sensor_config_topic_template, SENSOR_LITERAL, get_device_name(), "w");
    client->publish(topic, 0, true, mqtt_out_buf, out_len);


    sprintf_P(sensor_uniq_id, PSTR("%s-uptime"), get_device_name());
    out_len = autodiscovery_sprintf_config(mqtt_out_buf, sensor_uniq_id, dev_prefix_ptr, PSTR("Uptime"), PSTR("\"stat_t\":\"~uptime\",\"unit_of_meas\":\"s\",\"ic\":\"mdi:timer\",\"dev_cla\":\"duration\","));

    sprintf_P(topic, sensor_config_topic_template, SENSOR_LITERAL, get_device_name(), "u");
    client->publish(topic, 0, true, mqtt_out_buf, out_len);


#ifdef HTU21D_ENABLED
    sprintf_P(sensor_uniq_id, PSTR("%s-temp"), get_device_name());
    out_len = autodiscovery_sprintf_config(mqtt_out_buf, sensor_uniq_id, dev_prefix_ptr, PSTR("Temperature"), PSTR("\"stat_t\":\"~temp\",\"dev_cla\":\"temperature\",\"unit_of_meas\":\"°C\","));

    sprintf_P(topic, sensor_config_topic_template, SENSOR_LITERAL, get_device_name(), "t");
    client->publish(topic, 0, true, mqtt_out_buf, out_len);


    sprintf_P(sensor_uniq_id, PSTR("%s-hum"), get_device_name());
    out_len = autodiscovery_sprintf_config(mqtt_out_buf, sensor_uniq_id, dev_prefix_ptr, PSTR("Humidity"), PSTR("\"stat_t\":\"~hum\",\"dev_cla\":\"humidity\",\"unit_of_meas\":\"%\","));

    sprintf_P(topic, sensor_config_topic_template, SENSOR_LITERAL, get_device_name(), "h");
    client->publish(topic, 0, true, mqtt_out_buf, out_len);
#endif

    delete[] sensor_uniq_id;
    delete[] mqtt_out_buf;
    delete[] topic;

    if (dev_prefix)
        delete[] dev_prefix;

    send_sensors(true);
}

uint32_t mqtt_sensors_send_timer = 0;

const char* sensor_topic_template PROGMEM = "%s%s";

void send_sensors(bool force)
{
    if (millis() - mqtt_sensors_send_timer > 60000 || force)
    {
        if (mqtt_is_connected())
        {
            char* payload = new char[15];
            char* topic = new char[strlen(mqtt_topic_start()) + 10];

            sprintf_P(topic, sensor_topic_template, mqtt_topic_start(), "rssi");
            sprintf(payload, "%d", WiFi.RSSI());

            mqtt_get_client()->publish(topic, 0, false, payload);


            sprintf_P(topic, sensor_topic_template, mqtt_topic_start(), "uptime");
            sprintf(payload, "%lu", millis() / 1000);

            mqtt_get_client()->publish(topic, 0, false, payload);

#ifdef HTU21D_ENABLED
            sprintf_P(topic, sensor_topic_template, mqtt_topic_start(), "hum");
            sprintf(payload, "%.2f", htu21d_get_humidity());

            mqtt_get_client()->publish(topic, 0, false, payload);


            sprintf_P(topic, sensor_topic_template, mqtt_topic_start(), "temp");
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