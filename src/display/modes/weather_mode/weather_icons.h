#ifndef __WEATHER_ICONS_H__
#define __WEATHER_ICONS_H__

#include <Arduino.h>

uint8_t humidity_icon[] PROGMEM = { 0x00, 0x04, 0x0E, 0x1F, 0x1D, 0x19, 0x0E, 0x00 };
uint8_t temp_icon[] PROGMEM = { 0x0D, 0x0C, 0x0D, 0x0C, 0x0D, 0x1E, 0x1E, 0x0C };

uint8_t weather_icon_moon[32] PROGMEM =
{
    0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x07,
    0x00, 0x00, 0x00, 0x18, 0x10, 0x00, 0x00, 0x00,
    0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x18, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t weather_icon_cloud_up[16] PROGMEM =
{
    0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F,
    0x00, 0x00, 0x00, 0x18, 0x1C, 0x1C, 0x1E, 0x1F
};

uint8_t weather_icon_cloud_down_default[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t weather_icon_cloud_down_light_rain[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00
};

uint8_t weather_icon_cloud_down_mid_rain[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x02, 0x02, 0x02, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x04, 0x14, 0x14, 0x10, 0x00
};

uint8_t weather_icon_cloud_down_big_rain[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

uint8_t weather_icon_cloud_down_snow[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x12, 0x00, 0x04, 0x00, 0x00
};

uint8_t weather_icon_cloud_down_lightning[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x06, 0x0C, 0x08, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x06, 0x0C, 0x08, 0x00, 0x00
};

uint8_t weather_icon_cloud_down_fog[16] PROGMEM =
{
    0x1F, 0x0F, 0x00, 0x06, 0x0C, 0x08, 0x00, 0x00,
    0x1F, 0x1E, 0x00, 0x06, 0x0C, 0x08, 0x00, 0x00
};

uint8_t weather_icon_windy[32] PROGMEM =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x0C, 0x12, 0x02, 0x1C, 0x00,
    0x1F, 0x00, 0x1F, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x1E, 0x01, 0x01, 0x16, 0x10, 0x00, 0x00, 0x00
};

uint8_t weather_icon_sun[32] PROGMEM =
{
    0x00, 0x00, 0x00, 0x08, 0x03, 0x07, 0x17, 0x07,
    0x00, 0x10, 0x10, 0x02, 0x18, 0x1C, 0x1D, 0x1C,
    0x07, 0x03, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1C, 0x18, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00
};

#endif // __WEATHER_ICONS_H__