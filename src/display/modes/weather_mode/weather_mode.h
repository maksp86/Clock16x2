#ifndef __WEATHER_MODE_H__
#define __WEATHER_MODE_H__

#include "defines.h"

#ifdef USE_HOMEASSISTANT

#include <ArduinoJson.h>

#include "mqtt/mqtt.h"
#include "display/appearance/statusbar.h"
#include "display/modes/display_mode.h"
#include "display/scrolling_text/scrolling_text.h"
#include "helpers.h"
#include "weather_icons.h"

struct weather_forecast
{
    char cond[16];
    int8_t temperature;
    int8_t temperature_low;
};

struct weather_object {
    char cond[16];
    int8_t temperature;
    int8_t humidity;
    float wind_speed;
    weather_forecast forecast[2];
};

#define MODE_STATE_CURRENT_TEMP 0
#define MODE_STATE_FORECAST_FIRST 1
#define MODE_STATE_FORECAST_SECOND 2

class weather_mode : public DisplayMode
{
private:
    uint8_t mode_state;
    uint8_t mode_state_change_cnt;
    bool mode_state_setup = false;
    bool got_forecast = false;
    bool from_interaction = false;
    char text_down[32];
    uint8_t text_down_len;
    char* weather_topic = nullptr;
    weather_object weather_container;

    void load_icons(LiquidCrystal_I2C* lcd, const char* condition);
public:
    weather_mode();
    ~weather_mode();
    bool setup(LiquidCrystal_I2C* lcd, bool from_interaction);
    void update(LiquidCrystal_I2C* lcd);
    bool can_skip();
    bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len);
    uint32_t update_time();
    uint32_t show_time();
    void on_mqtt_data(char* topic, char* payload, size_t len);
    void mqtt_subscribe(AsyncMqttClient* client);
};

#endif

#endif // __WEATHER_MODE_H__