#ifndef __STATUSBAR_H__
#define __STATUSBAR_H__

#include "ESP8266WiFi.h"
#include "LiquidCrystal_I2C.h"

#include "helpers.h"
#include "defines.h"
#include "wifi/wifi.h"
#include "lock.h"

#define STATUSBAR_WIFI_ICON_POS 7
#define STATUSBAR_LOCK_ICON_POS 6

#define STATUSBAR_MODE_DEFAULT 0
#define STATUSBAR_MODE_SLIM 1

void statusbar_set_mode(LiquidCrystal_I2C* lcd, uint8_t mode);
void statusbar_set_wifi_busy();
void statusbar_update(LiquidCrystal_I2C* lcd);
void statusbar_full_redraw(LiquidCrystal_I2C* lcd);
#endif // __STATUSBAR_H__