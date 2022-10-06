#include "test_mode.h"

test_mode::test_mode(/* args */)
{
}

test_mode::~test_mode()
{
}

bool test_mode::setup(LiquidCrystal_I2C* lcd, bool from_interaction)
{
	lcd->clear();
	lcd->setCursor(0, 0);
	lcd->write("Test mode");
	return true;
}

void test_mode::update(LiquidCrystal_I2C* lcd)
{
	lcd->setCursor(7, 1);
	lcd_print_int(lcd, millis());
}

bool test_mode::can_skip()
{
	return true;
}

bool test_mode::on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len)
{
	lcd->setCursor(8, 1);
	for (uint8_t i = 0; i < 6; i++)
	{
		if (i < pattern_len)
		{
			lcd->write(pattern[i]);
		}
		else
		{
			lcd->write(' ');
		}
	}
	return true;
}

uint32_t test_mode::update_time()
{
	return 500;
}

uint32_t test_mode::show_time()
{
	return 6000;
}