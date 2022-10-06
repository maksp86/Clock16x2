#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "string.h"
#include "inttypes.h"
#include "LiquidCrystal_I2C.h"
#include <ESP8266WiFi.h>

#include "defines.h"

char byte_to_char(uint8_t value);
void lcd_print_int(LiquidCrystal_I2C* lcd, uint32_t value);
const char* get_device_name();
void make_device_name();
uint8_t get_percent_from_RSSI(int8_t rssi);
void load_PGM_character_to(LiquidCrystal_I2C* lcd, uint8_t place, const uint8_t* character);
uint8_t* hex2bin(const char* str);
#endif // __HELPERS_H__