#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"

#include "defines.h"

#include "appearance/statusbar.h"
#include "appearance/timed_backlight.h"
#include "appearance/lock.h"

#include "scrolling_text/scrolling_text.h"
#include "modes/display_mode.h"
//#include "modes/test_mode/test_mode.h"
#include "modes/progress_mode/progress_mode.h"
#include "modes/message_mode/message_mode.h"
#include "modes/time_mode/time_mode.h"

#ifdef HTU21D_ENABLED
#include "modes/local_climate_mode/local_climate_mode.h"
#endif

void display_setup();
void display_update();
bool display_show_mode(DisplayMode* mode);
bool display_add_mode(DisplayMode* mode);
bool display_enqueue_mode(DisplayMode* mode);
void display_on_interact(const char* pattern, uint8_t pattern_len);

#endif // __DISPLAY_H__