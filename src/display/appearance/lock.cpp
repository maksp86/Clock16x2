#include "lock.h"

char* lock_topic;
bool lock_state = false;

void on_lock_message(char* topic, char* payload, size_t len)
{
    if (strcmp(topic, lock_topic) == 0)
    {
        if (strncmp(payload, "true", 4) == 0)
        {
#if DEBUG >= 4
            Serial.println("on_lock_message locked");
#endif
            lock_state = true;
        }
        else if (strncmp(payload, "false", 5) == 0)
        {
          #if DEBUG >= 4
            Serial.println("on_lock_message unlocked");
#endif
            lock_state = false;
        }
    }
}

void subscribe_to_lock(AsyncMqttClient* client)
{
    lock_topic = new char[strlen(mqtt_topic_start()) + 12];
    sprintf(lock_topic, "%slock/set", mqtt_topic_start());
    client->subscribe(lock_topic, 0);
}

bool get_lock_state()
{
    return lock_state;
}