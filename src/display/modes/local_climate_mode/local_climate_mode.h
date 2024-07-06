#ifndef __LOCAL_CLIMATE_MODE__
#define __LOCAL_CLIMATE_MODE__

#include "defines.h"

#ifdef HTU21D_ENABLED

#include "htu21d/htu21d.h"
#include "display/modes/display_mode.h"
#include "display/appearance/statusbar.h"
#include "display/modes/weather_mode/weather_icons.h"
#include "helpers.h"

class local_climate_mode : public DisplayMode
{
private:
    bool mode_state_setup = false;
public:
    local_climate_mode();
    ~local_climate_mode();
    bool setup(LiquidCrystal_I2C* lcd, bool from_interaction);
    void update(LiquidCrystal_I2C* lcd);
    bool can_skip();
    bool on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len);
    uint32_t update_time();
    uint32_t show_time();
};

#endif

#endif // __LOCAL_CLIMATE_MODE__