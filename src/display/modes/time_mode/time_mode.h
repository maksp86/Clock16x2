#ifndef __TIME_MODE_H__
#define __TIME_MODE_H__

#include "avr/pgmspace.h"

#include "ntp/ntp.h"
#include "display/modes/display_mode.h"
#include "display/appearance/statusbar.h"
#include "helpers.h"

class time_mode : public DisplayMode
{
private:
    /* data */
    bool dots_visibility = true;
public:
    time_mode();
    ~time_mode();
    bool setup(LiquidCrystal_I2C* lcd, bool from_interaction);
    void update(LiquidCrystal_I2C* lcd);
    bool can_skip();
    bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len);
    uint32_t update_time();
    uint32_t show_time();
};

#endif // __TIME_MODE_H__