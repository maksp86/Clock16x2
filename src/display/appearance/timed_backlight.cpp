#include "timed_backlight.h"

uint32_t backlight_update_timer;

bool backlight_on_timer_armed = false;
bool backlight_state = true;
uint32_t backlight_on_timer;

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
    }

    if (backlight_on_timer_armed && millis() - backlight_on_timer > 10000)
    {
        backlight_on_timer_armed = false;
    }
}
