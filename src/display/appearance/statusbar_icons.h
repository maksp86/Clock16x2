#ifndef __STATUSBAR_ICONS_H__
#define __STATUSBAR_ICONS_H__

#include "avr/pgmspace.h"

const uint8_t wifi_state_icon[8] PROGMEM = {
0b01110,
0b10001,
0b00100,
0b01010,
0b00000,
0b00100,
0b00000,
0b00000
};

const uint8_t wifi_ap_icon[8] PROGMEM = {
0b01000,
0b10100,
0b11100,
0b10100,
0b00110,
0b00101,
0b00110,
0b00100
};

const uint8_t lock_state_icon[8] PROGMEM = { 0x00, 0x0E, 0x11, 0x11, 0x1F, 0x1B, 0x1B, 0x0E };

#endif // __STATUSBAR_ICONS_H__