#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <LittleFS.h>
#include <ArduinoJson.h>

#include "defines.h"

#define CONFIG_INACTIVITY_TIME_MS 10000

void config_loop();
void config_setup();
void config_unload();
bool config_load();

// bool config_set(const char* key, long value);
// bool config_set(const char* key, float value);
// bool config_set(const char* key, const char* value);
// bool config_set(const char* key, bool value);
bool config_get_long(const char* key, long* out);
bool config_get_bool(const char* key, bool* out);
bool config_get_str(const char* key, char** out);
bool config_get_float(const char* key, float* out);

#endif // __CONFIG_H__