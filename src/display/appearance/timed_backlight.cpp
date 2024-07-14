#include "timed_backlight.h"

uint32_t backlight_update_timer;

bool backlight_on_timer_armed = false;
bool backlight_state = true;
uint32_t backlight_on_timer;

uint8_t backlight_night_mode_time[4] = { 24,0,0,0 };
#define BACKLIGHT_NIGHTMODE_START_HR backlight_night_mode_time[0]
#define BACKLIGHT_NIGHTMODE_START_MIN backlight_night_mode_time[1]
#define BACKLIGHT_NIGHTMODE_END_HR backlight_night_mode_time[2]
#define BACKLIGHT_NIGHTMODE_END_MIN backlight_night_mode_time[3]

void backlight_night_mode_arm(LiquidCrystal_I2C* lcd)
{
    if (get_lock_state())
        return;

    backlight_state = true;
    backlight_on_timer_armed = true;
    backlight_on_timer = millis();
    lcd->setBacklight(backlight_state);
}

bool backlight_get_state()
{
    return backlight_state;
}

void backlight_setup(LiquidCrystal_I2C* lcd)
{
    char* bl_conf_str;
    if (config_get_str("night_mode_time", &bl_conf_str))
    {
        if (strchrs(bl_conf_str, ':') == 2 && strchrs(bl_conf_str, '-') == 1)
        {
            BACKLIGHT_NIGHTMODE_START_HR = atoi(bl_conf_str);

            char* next_ptr = strchr(bl_conf_str, ':') + 1;

            BACKLIGHT_NIGHTMODE_START_MIN = atoi(next_ptr);
            next_ptr = strchr(next_ptr, '-') + 1;

            BACKLIGHT_NIGHTMODE_END_HR = atoi(next_ptr);
            next_ptr = strchr(next_ptr, ':') + 1;

            BACKLIGHT_NIGHTMODE_END_MIN = atoi(next_ptr);

#if DEBUG >= 4
            Serial.printf_P(PSTR("night_mode_time (%s) parsed to %d:%d-%d:%d\n"), bl_conf_str, BACKLIGHT_NIGHTMODE_START_HR, BACKLIGHT_NIGHTMODE_START_MIN, BACKLIGHT_NIGHTMODE_END_HR, BACKLIGHT_NIGHTMODE_END_MIN);
#endif
        }
        delete[] bl_conf_str;
    }
}

void backlight_update(LiquidCrystal_I2C* lcd)
{
    if (!backlight_on_timer_armed && ntp_time_set() && millis() - backlight_update_timer > 1000)
    {
        tm* tm_ptr = localtime(ntp_get_epoch_time());
        if (((tm_ptr->tm_hour == BACKLIGHT_NIGHTMODE_START_HR && tm_ptr->tm_min >= BACKLIGHT_NIGHTMODE_START_MIN) || tm_ptr->tm_hour > BACKLIGHT_NIGHTMODE_START_HR) ||
            ((tm_ptr->tm_hour == BACKLIGHT_NIGHTMODE_END_HR && tm_ptr->tm_min <= BACKLIGHT_NIGHTMODE_END_MIN) || tm_ptr->tm_hour < BACKLIGHT_NIGHTMODE_END_HR) || get_lock_state())
        {
            if (backlight_state)
            {
                backlight_state = false;
                lcd->setBacklight(backlight_state);
            }
        }
        else
        {
            if (!backlight_state)
            {
                backlight_state = true;
                lcd->setBacklight(backlight_state);
            }
        }
        backlight_update_timer = millis();
    }

    if (backlight_on_timer_armed && millis() - backlight_on_timer > 10000)
    {
        backlight_on_timer_armed = false;
    }
}
