#ifndef __TIMED_BACKLIGHT_H__
#define __TIMED_BACKLIGHT_H__

#include "config/config.h"
#include "ntp/ntp.h"
#include "display/display.h"
#include "display/appearance/lock.h"

// #define BACKLIGHT_NIGHTMODE_START_HR 23
// #define BACKLIGHT_NIGHTMODE_START_MIN 30
// #define BACKLIGHT_NIGHTMODE_END_HR 7
// #define BACKLIGHT_NIGHTMODE_END_MIN 0

void backlight_update(LiquidCrystal_I2C* lcd);
void backlight_setup(LiquidCrystal_I2C* lcd);
void backlight_night_mode_arm(LiquidCrystal_I2C* lcd);
bool backlight_get_state();
#endif // __TIMED_BACKLIGHT_H__