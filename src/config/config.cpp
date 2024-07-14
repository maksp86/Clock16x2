#include "config.h"

#define CONFIG_VALUE_TYPE_STRING 1
#define CONFIG_VALUE_TYPE_NUMBER 2
#define CONFIG_VALUE_TYPE_FLOAT 3
#define CONFIG_VALUE_TYPE_BOOLEAN 4

bool config_loaded = false;
uint32_t config_inactivity_timer = 0;

JsonDocument doc;

void config_rearm_timer()
{
    config_inactivity_timer = millis();
}

bool config_load()
{
    if (config_loaded)
    {
        config_rearm_timer();
        return true;
    }

    config_loaded = false;
    if (!LittleFS.begin())
        return false;

    if (LittleFS.exists(CONFIG_FILENAME))
    {
        File file = LittleFS.open(CONFIG_FILENAME, "r");

        DeserializationError err = deserializeJson(doc, file);
        if (err)
        {
#if DEBUG >= 4
            Serial.print(F("config_load err: "));
            Serial.println(err.f_str());
#endif
            return false;
        }
    }
    else
    {
#if DEBUG >= 4
        Serial.println(F("config_load no file"));
#endif
    }
    config_rearm_timer();
    config_loaded = true;
    return true;
}

void config_unload()
{
    if (!config_loaded)
        return;
    config_loaded = false;

    File file = LittleFS.open(CONFIG_FILENAME, "w");

    size_t size = serializeJsonPretty(doc, file);
    if (size == 0)
    {
#if DEBUG >= 4
        Serial.println(F("config_unload err"));
#endif
    }
    else
    {
#if DEBUG >= 4
        Serial.print(F("config_unload size "));
        Serial.println(size);
#endif
        doc.clear();
    }
}

void config_loop()
{
    if (config_loaded && millis() - config_inactivity_timer > CONFIG_INACTIVITY_TIME_MS)
    {
        config_unload();
    }
}

void config_setup()
{
    config_load();
}

// bool config_set(const char* key, const char* value)
// {
//     if (!config_load())
//         return false;

//     doc[key] = value;
//     return true;
// }

// bool config_set(const char* key, long value)
// {
//     if (!config_load())
//         return false;

//     doc[key] = value;
//     return true;
// }

// bool config_set(const char* key, float value)
// {
//     if (!config_load())
//         return false;

//     doc[key] = value;
//     return true;
// }

// bool config_set(const char* key, bool value)
// {
//     if (!config_load())
//         return false;

//     doc[key] = value;
//     return true;
// }

bool config_get_str(const char* key, char** out)
{
    if (!config_load())
        return false;

    const char* value = doc[key]["v"];
    if (!value)
    {
        JsonObject obj = doc[key].to<JsonObject>();
        obj["t"] = CONFIG_VALUE_TYPE_STRING;
        return false;
    }

    size_t len = strlen(value) + 1;
    *out = new char[len];
    memcpy(*out, value, len - 1);
    (*out)[len - 1] = '\0';

    return true;
}

bool config_get_long(const char* key, long* out)
{
    if (config_load())
    {
        if (!doc.containsKey(key))
        {
            JsonObject obj = doc[key].to<JsonObject>();
            obj["t"] = CONFIG_VALUE_TYPE_NUMBER;
        }
        else if (doc[key]["v"])
        {
            *out = doc[key]["v"].as<long>();
            return true;
        }
    }
    return false;
}

bool config_get_bool(const char* key, bool* out)
{
    if (config_load())
    {
        if (!doc.containsKey(key))
        {
            JsonObject obj = doc[key].to<JsonObject>();
            obj["t"] = CONFIG_VALUE_TYPE_BOOLEAN;
        }
        else if (doc[key]["v"])
        {
            *out = doc[key]["v"].as<bool>();
            return true;
        }
    }
    return false;
}

bool config_get_float(const char* key, float* out)
{
    if (config_load())
    {
        if (!doc.containsKey(key))
        {
            JsonObject obj = doc[key].to<JsonObject>();
            obj["t"] = CONFIG_VALUE_TYPE_FLOAT;
        }
        else if (doc[key]["v"])
        {
            *out = doc[key]["v"].as<float>();
            return true;
        }
    }
    return false;
}
