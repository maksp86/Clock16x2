#include "mqtt.h"

AsyncMqttClient* mqtt_client;

char* mqtt_host = nullptr;
long mqtt_port = 0;
char* mqtt_user = nullptr;
char* mqtt_password = nullptr;

bool mqtt_reconnect_timer_armed;
uint32_t mqtt_reconnect_timer;
uint16_t mqtt_reconnect_time;

char* mqtt_willtopic;
void mqtt_disconnected_callback(AsyncMqttClientDisconnectReason reason);
void mqtt_connected_callback(bool sessionPresent);

char* mqtt_device_topic;
void mqtt_setup()
{
    mqtt_reconnect_time = 10000;
    mqtt_client = new AsyncMqttClient();
    mqtt_client->onConnect(&mqtt_connected_callback);
    mqtt_client->onDisconnect(&mqtt_disconnected_callback);
    mqtt_client->onMessage(&mqtt_on_message);
    mqtt_reconnect_timer_armed = false;
    mqtt_willtopic = new char[64];

    mqtt_device_topic = new char[strlen(get_device_name()) + 30 + 1];
    sprintf_P(mqtt_device_topic, PSTR(MQTT_TOPIC_PREFIX), get_device_name());
}

void mqtt_update()
{
    if (mqtt_reconnect_timer_armed && millis() - mqtt_reconnect_timer > mqtt_reconnect_time)
    {
#if DEBUG >= 4
        Serial.println("mqtt_reconnect");
#endif
        mqtt_client->connect();
        mqtt_reconnect_timer = millis();
    }

}

const char* state_topic_template PROGMEM = "%sstate";

void mqtt_connect(const char* login, const char* password)
{
    if (mqtt_is_connected())
        return;

    mqtt_client->setClientId(get_device_name());
    mqtt_client->setCredentials(login, password);
    sprintf_P(mqtt_willtopic, state_topic_template, mqtt_topic_start());
#if DEBUG >= 4
    Serial.printf("mqtt_connect: %s\n", mqtt_willtopic);
#endif
    mqtt_client->setWill(mqtt_willtopic, 0, true, "offline");

    mqtt_client->connect();
    statusbar_set_wifi_busy();
}

void mqtt_connect()
{
    mqtt_disconnect(true);
    if (!config_get_str("mqtt_host", &mqtt_host) || !config_get_long("mqtt_port", &mqtt_port))
        return;

    config_get_str("mqtt_user", &mqtt_user);
    config_get_str("mqtt_pass", &mqtt_password);

#if DEBUG >= 4
    Serial.printf("mqtt_connect %s %u %s %s\n", mqtt_host, mqtt_port, mqtt_user, mqtt_password);
#endif

    mqtt_client->setServer(mqtt_host, mqtt_port);
    mqtt_connect(mqtt_user, mqtt_password);
}

// void mqtt_connect(IPAddress server, uint16_t port, const char* login, const char* password)
// {
//     mqtt_client->setServer(server, port);
//     mqtt_connect(login, password);

// }

// void mqtt_connect(const char* server, uint16_t port, const char* login, const char* password)
// {
//     mqtt_client->setServer(server, port);
//     mqtt_connect(login, password);
// }

void mqtt_disconnect(bool force)
{
    mqtt_client->disconnect(force);
    mqtt_reconnect_timer_armed = false;

    if (mqtt_host)
        delete[] mqtt_host;

    if (mqtt_user)
        delete[] mqtt_user;

    if (mqtt_password)
        delete[] mqtt_password;

    mqtt_host = mqtt_user = mqtt_password = nullptr;
}

bool mqtt_is_connected()
{
    return mqtt_client->connected();
}

AsyncMqttClient* mqtt_get_client()
{
    return mqtt_client;
}

const char* mqtt_topic_start()
{
    return mqtt_device_topic;
}


void mqtt_disconnected_callback(AsyncMqttClientDisconnectReason reason)
{
#if DEBUG >= 4
    Serial.printf("mqtt_disconnected_callback %d\n", (uint8_t)reason);
#endif
    switch (reason)
    {
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
        mqtt_reconnect_timer_armed = false;
        break;

    default:
        if (wifi_get_last_status() == wifi_status::WIFI_CONNECTED)
        {
            mqtt_reconnect_timer_armed = true;
            mqtt_reconnect_timer = millis();
        }
        else
            mqtt_reconnect_timer_armed = false;
        break;
    }
}

void mqtt_connected_callback(bool sessionPresent)
{
#if DEBUG >= 4
    Serial.printf("mqtt_connected_callback %d\n", sessionPresent);
#endif
    mqtt_reconnect_timer_armed = false;

    char* topicbuf = new char[64];
    sprintf_P(topicbuf, state_topic_template, mqtt_topic_start());

    mqtt_client->publish(topicbuf, 0, true, "online");
    delete[] topicbuf;

    mqtt_on_connected(mqtt_client);
}
