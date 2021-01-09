#ifndef HELPERS_CONFIG_HELPER_H
#define HELPERS_CONFIG_HELPER_H

#include <WiFiManager.h>
#include <ArduinoJson.h>

class configHelper
{
private:
    bool Beginned;
    DynamicJsonDocument json{ 256 };
    WiFiManager* manager;
public:
    configHelper(WiFiManager* man);
    ~configHelper();
    bool Load();
    bool Save();
    const char* GetValue(const char* key);
    const char* GetValueOrDefault(const char* key, const char* defvalue);
    void AddProperty(const char* name, const char* description, const char* defvalue);
    void AddProperty(char* description);
};

#endif