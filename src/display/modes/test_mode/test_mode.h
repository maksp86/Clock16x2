#ifndef __TEST_MODE_H__
#define __TEST_MODE_H__

#include "display/modes/display_mode.h"
#include "helpers.h"

class test_mode : public DisplayMode
{
private:
    /* data */
public:
    test_mode();
    ~test_mode();
    bool setup(LiquidCrystal_I2C* lcd, bool from_interaction);
    void update(LiquidCrystal_I2C* lcd);
    bool can_skip();
    bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len);
    uint32_t update_time();
    uint32_t show_time();
};

#endif // __TEST_MODE_H__