#include "lock.h"

char* lock_topic = nullptr;
bool lock_state = false;

void on_lock_message(char* topic, char* payload, size_t len)
{
    if (strcmp(topic, lock_topic) == 0)
    {
        if (len > 1 && payload[0] == 'O')
        {
            lock_state = payload[1] == 'N';
#if DEBUG >= 4
            Serial.printf("on_lock_message %slocked", payload[1] == 'N' ? "un" : "");
#endif
        }
    }
}

void subscribe_to_lock(AsyncMqttClient* client)
{
    if (lock_topic == nullptr)
    {
        lock_topic = new char[strlen(mqtt_topic_start()) + 12];
        sprintf(lock_topic, "%slock/set", mqtt_topic_start());
    }
    client->subscribe(lock_topic, 0);
}

bool get_lock_state()
{
    return lock_state;
}