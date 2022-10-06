#ifndef __DISPLAY_MODE_H__
#define __DISPLAY_MODE_H__

#include "inttypes.h"
#include "LiquidCrystal_I2C.h"

class DisplayMode
{
public:
    virtual bool setup(LiquidCrystal_I2C* lcd, bool from_interaction) = 0;
    virtual void update(LiquidCrystal_I2C* lcd) = 0;
    virtual bool can_skip() = 0;
    virtual bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len) = 0;
    virtual uint32_t update_time() = 0;
    virtual uint32_t show_time() = 0;
    virtual ~DisplayMode() {}
};

#endif // __DISPLAY_MODE_H__