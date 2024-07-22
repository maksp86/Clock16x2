#ifndef __OTA_H__
#define __OTA_H__

#include <Arduino.h>
#include <ArduinoOTA.h>

#include "helpers.h"
#include "display/display.h"
#include "display/modes/progress_mode/progress_mode.h"
#include "display/modes/message_mode/message_mode.h"
#include "wifi/wifi.h"
#include "config/config.h"

void ota_setup();
void ota_update();

void ota_on_start();
void ota_on_end();
void ota_on_progress(unsigned int progress, unsigned int total);
void ota_on_error(ota_error_t error);
#endif // __OTA_H__