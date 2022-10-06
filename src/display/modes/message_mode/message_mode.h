#ifndef __MESSAGE_MODE_H__
#define __MESSAGE_MODE_H__

#include "display/modes/display_mode.h"
#include "display/scrolling_text/scrolling_text.h"
#include "display/appearance/statusbar.h"
#include "helpers.h"

#define ICON_SIZE (8*4)

class message_mode : public DisplayMode
{
private:
    char* title;
    uint8_t title_len;
    char* message;
    uint8_t message_len;

    uint8_t* icon_arr;
    uint32_t _show_time;
    bool _can_skip;
public:
    //icon from array
    message_mode(const char* title, uint8_t title_len, const char* message, uint8_t message_len, const uint8_t* icon, uint32_t show_time, bool can_skip);
    //icon from PROGMEM
    message_mode(PGM_P title, PGM_P message = NULL, const byte** icon = NULL, uint32_t show_time = 2000, bool can_skip = true);
    ~message_mode();
    bool setup(LiquidCrystal_I2C* lcd, bool from_interaction);
    void update(LiquidCrystal_I2C* lcd);
    bool can_skip();
    bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len);
    uint32_t update_time();
    uint32_t show_time();
};

#endif // __MESSAGE_MODE_H__