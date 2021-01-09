#define _DEBUG

#include <WiFiManager.h>

#include "helpers/configHelper.h"
#include "helpers/debugHelper.h"
#include <LittleFS.h>

const char* ID_KEY = "id";
const char* PLACEHOLDER_KEY = "ph";
const char* DEFAULT_VALUE_KEY = "df";
const char* PARAMETERS_KEY = "ps";
const int JSON_SIZE = 512;

configHelper::configHelper(WiFiManager* man)
{
    manager = man;
#ifndef _DEBUG
    DebugHelper::Log("configHelper", "init");
#endif

#ifdef _DEBUG
    DebugHelper::Log("configHelper", "mount");
#endif
    if (LittleFS.begin())
    {
#ifdef _DEBUG
        DebugHelper::Log("configHelper", "mount ok");
#endif
        Beginned = true;
    }
    else
    {
#ifdef _DEBUG
        DebugHelper::Log("configHelper", "mount err");
#endif
        Beginned = false;
    }

}

configHelper::~configHelper()
{
    if (Beginned)
        LittleFS.end();
    json.clear();
}

bool configHelper::Load()
{
    bool result = false;
    if (Beginned)
    {
        if (LittleFS.exists("/config.json"))
        {
            File file = LittleFS.open("/config.json", "r");
            if (file)
            {
#ifdef _DEBUG
                DebugHelper::Log("configHelper", "file open ok");
#endif
                DeserializationError err = deserializeJson(json, file);
                serializeJson(json, Serial);
                if (err.code() == DeserializationError::Ok)
                {
#ifdef _DEBUG
                    DebugHelper::Log("configHelper: json", "read ok");
#endif
                    result = true;
                }
                else
                {
                    result = true;
#ifdef _DEBUG
                    switch (err.code())
                    {
                    case DeserializationError::InvalidInput:
                        DebugHelper::Log("configHelper: json", "Invalid input");
                        break;
                    case DeserializationError::NoMemory:
                        DebugHelper::Log("configHelper: json", "Not enough memory");
                        break;
                    default:
                        DebugHelper::Log("configHelper: json", "Deserialization failed");
                        break;
                    }
#endif
                }
                file.close();
            }
            else
            {
#ifdef _DEBUG
                DebugHelper::Log("configHelper", "file open failed");
#endif
                result = false;
            }
        }
        else
        {
#ifdef _DEBUG
            DebugHelper::Log("configHelper", "file not exist");
#endif
            result = true;
        }

    }
    return result;
}

bool configHelper::Save()
{
    bool result = false;
    if (Beginned && manager->getParametersCount() > 0)
    {
#ifdef _DEBUG
        DebugHelper::Log("configHelper", "saving");
#endif
        WiFiManagerParameter** params = manager->getParameters();
        for (int i = 0; i < manager->getParametersCount(); i++)
        {
            WiFiManagerParameter* param = params[i];
#ifdef _DEBUG
            DebugHelper::Log("configHelper", param->getID());
#endif
            if (!param->getID())
                continue;
            json[param->getID()] = param->getValue();
        }

        File file = LittleFS.open("/config.json", "w");
        if (file)
        {
            if (serializeJson(json, file) == 0)
            {
#ifdef _DEBUG
                DebugHelper::Log("configHelper", "file write failed");
#endif
                result = false;
            }
            else
            {
                serializeJson(json, Serial);
#ifdef _DEBUG
                DebugHelper::Log("configHelper", "saving ok");
#endif
                result = true;
            }

            file.close();
        }
        else
        {
#ifdef _DEBUG
            DebugHelper::Log("configHelper", "file open failed");
#endif
        }
    }
    return result;
}

void configHelper::AddProperty(const char* id, const char* description, const char* defvalue)
{
#ifdef _DEBUG
    DebugHelper::Log("configHelper", "AddProperty");
#endif
    WiFiManagerParameter* param = new WiFiManagerParameter(id, description, GetValueOrDefault(id, defvalue), 32);
    manager->addParameter(param);
}

void configHelper::AddProperty(char* description)
{
    WiFiManagerParameter* param = new WiFiManagerParameter(description);
    manager->addParameter(param);
}

const char* configHelper::GetValue(const char* key)
{
#ifdef _DEBUG
    DebugHelper::Log("configHelper", "GetValue");
    DebugHelper::Log("configHelper", key);
#endif
    if (json.containsKey(key))
        return json[key];
    else
        return NULL;
}

const char* configHelper::GetValueOrDefault(const char* key, const char* defval)
{
#ifdef _DEBUG
    DebugHelper::Log("configHelper", "GetValueOrDefault");
    DebugHelper::Log("configHelper", key);
    DebugHelper::Log("configHelper", defval);
#endif
    if (json.containsKey(key))
        return json[key];
    else
        return defval;
}