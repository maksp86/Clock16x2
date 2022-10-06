#include "weather_mode.h"
#include "weather_icons.h"


weather_mode::weather_mode()
{
}

weather_mode::~weather_mode()
{
    delete[] weather_topic;
}

bool weather_mode::setup(LiquidCrystal_I2C* lcd, bool _from_interaction)
{
    if (!got_forecast) return false;
    mode_state_setup = false;
    mode_state = 0;
    mode_state_change_cnt = 0;
    from_interaction = _from_interaction;
    return true;
}

void weather_mode::update(LiquidCrystal_I2C* lcd)
{
    if (!mode_state_setup)
    {
        lcd->clear();
        statusbar_set_mode(lcd, STATUSBAR_MODE_DEFAULT);
        load_icons(lcd, mode_state == 0 ? weather_container.cond : weather_container.forecast[mode_state - 1].cond);
        load_PGM_character_to(lcd, 4, temp_icon);
        load_PGM_character_to(lcd, 5, humidity_icon);

        lcd->setCursor(0, 0);
        lcd->write((uint8_t)0);
        lcd->write((uint8_t)1);
        lcd->setCursor(0, 1);
        lcd->write((uint8_t)2);
        lcd->write((uint8_t)3);

        lcd->setCursor(2, 0);

        if (mode_state == 0)
        {
            lcd->write(weather_container.cond);
            text_down_len = sprintf(text_down, "%c%d%cC %c%d%%", (char)4, weather_container.temperature, (char)223, (char)5, weather_container.humidity);
        }
        else
        {
            lcd->write(weather_container.forecast[mode_state - 1].cond);
            text_down_len = sprintf(text_down, "%c%d%cC/%d%cC", (char)4, weather_container.forecast[mode_state - 1].temperature, (char)223, weather_container.forecast[mode_state - 1].temperature_low, (char)223);
        }
        if (text_down_len > 14)
        {
            scrolling_text_prepare(text_down, text_down_len, 5, 2, 1, 14);
        }
        else
        {
            lcd->setCursor(2, 1);
            lcd->write(text_down);
        }
        mode_state_setup = true;
    }

    if (mode_state_change_cnt < 20)
    {
        mode_state_change_cnt++;
    }
    else
    {
        if (from_interaction && mode_state < 2)
        {
            mode_state_change_cnt = 0;
            mode_state++;
            mode_state_setup = false;
        }
    }

    if (text_down_len > 14)
    {
        scrolling_text_loop(lcd);
    }
}

bool weather_mode::can_skip()
{
    return true;
}

bool weather_mode::on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len)
{
    if (from_interaction && pattern_len == 1 && pattern[0] == 's')
    {
        if (mode_state < 2)
        {
            mode_state_change_cnt = 0;
            mode_state++;
            mode_state_setup = false;
            return true;
        }
    }
    return false;
}

uint32_t weather_mode::update_time()
{
    return 200;
}

uint32_t weather_mode::show_time()
{
    return from_interaction ? 12000 : 4000;
}

void weather_mode::on_mqtt_data(char* topic, char* payload, size_t len)
{
    if (strcmp(weather_topic, topic) == 0)
    {
        StaticJsonDocument<256> forecast_json;
        DeserializationError error = deserializeJson(forecast_json, payload, len);

        if (error) {
#if DEBUG >= 4
            Serial.print(F("weather_mode deserialize fail: "));
            Serial.println(error.f_str());
#endif
            return;
        }

        strcpy(weather_container.cond, forecast_json["cond"]);
        weather_container.temperature = forecast_json["temp"];
        weather_container.humidity = forecast_json["hum"];
        weather_container.wind_speed = forecast_json["wndspd"];

        JsonArray forecast_arr = forecast_json["forecast"];
        for (uint8_t i = 0; i < 2; i++) {

            strcpy(weather_container.forecast[i].cond, forecast_arr[i]["cond"]);
            weather_container.forecast[i].temperature = forecast_arr[i]["temp"];
            weather_container.forecast[i].temperature_low = forecast_arr[i]["templow"];
        }
        got_forecast = true;
    }
}

void weather_mode::mqtt_subscribe(AsyncMqttClient* client)
{
    weather_topic = new char[strlen(mqtt_topic_start()) + 20];
    sprintf(weather_topic, "%syaweather/status", mqtt_topic_start());
    client->subscribe(weather_topic, 0);
}

void weather_mode::load_icons(LiquidCrystal_I2C* lcd, const char* condition)
{
    uint8_t* icon_first_part = weather_icon_cloud_up;
    uint8_t* icon_last_part = weather_icon_cloud_down_default;

    if (strncmp(condition, "snowy", 5) == 0)
    {
        icon_last_part = weather_icon_cloud_down_snow;
    }
    else if (strncmp(condition, "windy", 5) == 0)
    {
        icon_first_part = weather_icon_windy;
        icon_last_part = weather_icon_windy + 16;
    }
    else if (strncmp(condition, "sunny", 5) == 0)
    {
        icon_first_part = weather_icon_sun;
        icon_last_part = weather_icon_sun + 16;
    }
    else if (strncmp(condition, "clear", 5) == 0)
    {
        icon_first_part = weather_icon_moon;
        icon_last_part = weather_icon_moon + 16;
    }
    else if (strncmp(condition, "light", 5) == 0)
    {
        icon_last_part = weather_icon_cloud_down_lightning;
    }
    else if (strncmp(condition, "pouring", 7) == 0)
    {
        icon_last_part = weather_icon_cloud_down_big_rain;
    }
    else if (strncmp(condition, "rainy", 5) == 0)
    {
        icon_last_part = weather_icon_cloud_down_mid_rain;
    }
    else if (strncmp(condition, "fog", 3) == 0)
    {
        icon_last_part = weather_icon_cloud_down_fog;
    }

    load_PGM_character_to(lcd, 0, icon_first_part);
    load_PGM_character_to(lcd, 1, icon_first_part + 8);
    load_PGM_character_to(lcd, 2, icon_last_part);
    load_PGM_character_to(lcd, 3, icon_last_part + 8);
}
