#include "local_climate_mode.h"

#ifdef HTU21D_ENABLED

local_climate_mode::local_climate_mode(/* args */)
{
}

local_climate_mode::~local_climate_mode()
{
}

bool local_climate_mode::setup(LiquidCrystal_I2C* lcd, bool from_interaction)
{
	if (htu21d_is_connected())
	{
		lcd->clear();
		statusbar_set_mode(lcd, STATUSBAR_MODE_DEFAULT);
		load_PGM_character_to(lcd, 1, temp_icon);
		load_PGM_character_to(lcd, 2, humidity_icon);

		lcd->setCursor(0, 0);
		lcd->write_P(PSTR("Room climate:"));

		char* text = new char[32];
		uint8_t text_len = sprintf_P(text, PSTR("%c%.1f%cC %c%.1f%%"), (char)1, htu21d_get_temperature(), (char)223, (char)2, htu21d_get_humidity());

		lcd->setCursor(0, 1);
		lcd->write(text);

		delete[] text;
		return true;
	}
	else
		return false;
}

void local_climate_mode::update(LiquidCrystal_I2C* lcd)
{

}

bool local_climate_mode::can_skip()
{
	return true;
}

bool local_climate_mode::on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len)
{
	return false;
}

uint32_t local_climate_mode::update_time()
{
	return 1000;
}

uint32_t local_climate_mode::show_time()
{
	return 5000;
}

#endif