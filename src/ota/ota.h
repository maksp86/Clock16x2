#ifndef __OTA_H__
#define __OTA_H__

#include <Arduino.h>
#include <ArduinoOTA.h>

#include "helpers.h"
#include "display/display.h"
#include "display/modes/progress_mode/progress_mode.h"
#include "display/modes/message_mode/message_mode.h"
#include "wifi/wifi.h"

void ota_setup();
void ota_update();
#endif // __OTA_H__