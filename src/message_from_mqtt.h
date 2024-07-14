#ifndef __MESSAGE_FROM_MQTT_H__
#define __MESSAGE_FROM_MQTT_H__

#include <ArduinoJson.h>

#include "display/display.h"
#include "display/modes/message_mode/message_mode.h"
#include "mqtt/mqtt.h"

char* message_topic = nullptr;

void check_message(char* topic, char* payload, size_t len)
{
    if (strcmp(topic, message_topic) == 0)
    {
        JsonDocument message_json;
        DeserializationError error = deserializeJson(message_json, payload, len);
        if (error) {
#if DEBUG >= 4
            Serial.print(F("check_message deserializeJson() failed: "));
            Serial.println(error.f_str());
#endif
            return;
        }
        const char* title = message_json["title"];
        const char* message = message_json["message"];
        const char* icon = message_json["icon"];
        uint8_t message_len = 0, title_len = 0;

        if (title)
            title_len = strlen(title);
        else
            title = NULL;

        if (message)
            message_len = strlen(message);
        else
            message = NULL;

        uint8_t* iconarr = NULL;;
        if (icon && strlen(icon) >= 64)
        {
#if DEBUG >= 4
            Serial.println(icon);
#endif
            iconarr = hex2bin(icon);
        }

        int time = 2000;
        if (message_json.containsKey("time"))
            time = message_json["time"];

        bool enqueue = true;
        if (message_json.containsKey("enqueue"))
        {
            enqueue = message_json["enqueue"];
        }
        message_mode* mode = new message_mode(title, title_len, message, message_len, iconarr, time, true);
        if (enqueue)
            display_enqueue_mode(mode);
        else
            display_show_mode(mode);

        if (iconarr != NULL)
            free(iconarr);

        message_json.clear();
    }
}

void subscribe_to_message(AsyncMqttClient* client)
{
    if (message_topic == nullptr)
    {
        message_topic = new char[strlen(mqtt_topic_start()) + 10];
        sprintf(message_topic, "%smessage", mqtt_topic_start());
    }
    client->subscribe(message_topic, 0);
}
#endif // __MESSAGE_FROM_MQTT_H__