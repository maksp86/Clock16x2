#ifndef __HTU21D_H__
#define __HTU21D_H__

#include "defines.h"

#ifdef HTU21D_ENABLED
#include "Arduino.h"

#include "Wire.h"
#include "SHT2x.h"

void htu21d_setup();
void htu21d_update();

float htu21d_get_temperature();
float htu21d_get_humidity();

bool htu21d_is_connected();
#endif

#endif // __HTU21D_H__