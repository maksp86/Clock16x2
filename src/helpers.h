#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "string.h"
#include "inttypes.h"
#include "LiquidCrystal_I2C.h"
#include <ESP8266WiFi.h>

#include "defines.h"
#include "config/config.h"

char byte_to_char(uint8_t value);
void generate_random_string(char* out, size_t len);
void lcd_print_int(LiquidCrystal_I2C* lcd, uint32_t value);
const char* get_device_name();
void make_device_name();
uint8_t get_percent_from_RSSI(int8_t rssi);
void load_PGM_character_to(LiquidCrystal_I2C* lcd, uint8_t place, const uint8_t* character);

/**
 * @brief Convert hex string to byte array 
 * @details memory is allocated by function itself, it is necessary to free memory after using
 * @param str hex string
 * @return uint8_t* byte array
 */
uint8_t* hex2bin(const char* str);

/**
 * @brief Count all occurences of chr in str
 * 
 * @param str a string to count char occurences in
 * @param chr char
 * @return int number of occurences
 */
int strchrs(char* str, char chr);

bool is_need_restart();
void set_need_restart();
#endif // __HELPERS_H__