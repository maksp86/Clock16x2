#ifndef __BUTTON_H__
#define __BUTTON_H__

#define MAX_PATTERN_LEN 10

#include "Arduino.h"
#include "defines.h"
void button_setup();
void button_update();
void button_callback(const char* pattern, uint8_t pattern_len, uint32_t press_time);

#endif // __BUTTON_H__