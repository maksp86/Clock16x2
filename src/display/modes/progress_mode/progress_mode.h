#ifndef __PROGRESS_MODE_H__
#define __PROGRESS_MODE_H__

#include "display/scrolling_text/scrolling_text.h"
#include "display/modes/display_mode.h"
#include "display/appearance/statusbar.h"
#include "helpers.h"

class progress_mode : public DisplayMode
{
private:
    char* title;
    uint8_t title_len;
    bool is_intermidate;
    unsigned int min_value;
    unsigned int max_value;
    uint8_t last_value;
    unsigned int value;
    uint8_t* curr_cell;
    void update_intermidate(LiquidCrystal_I2C *lcd);
    void update_normal(LiquidCrystal_I2C *lcd);
public:
    progress_mode(char* title, uint8_t title_len, bool is_intermidate, unsigned int min_value, unsigned int max_value);
    progress_mode(PGM_P title, bool is_intermidate, unsigned int min_value, unsigned int max_value);
    ~progress_mode();
    bool setup(LiquidCrystal_I2C* lcd, bool from_interaction);
    void update(LiquidCrystal_I2C* lcd);
    void set_progress(unsigned int value);
    bool can_skip();
    bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len);
    uint32_t update_time();
    uint32_t show_time();
};

#endif // __PROGRESS_MODE_H__