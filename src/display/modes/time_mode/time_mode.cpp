#include "time_mode.h"

uint8_t time_lastMin, time_lastHr;

const uint8_t CustomChars[6][8] PROGMEM =
{
    {0b11111,0b11111,0b11111,0b00000,0b00000,0b00000,0b00000,0b00000},
    {0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111},
    {0b11111,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111,0b11111},
    {0b11111,0b11111,0b11111,0b00000,0b00000,0b00000,0b00000,0b11111},
    {0b01111,0b01111,0b01111,0b01111,0b01111,0b01111,0b01111,0b01111},
    {0b11110,0b11110,0b11110,0b11110,0b11110,0b11110,0b11110,0b11110}
};

#define BLACK 255
#define EMPTY 254

const byte BigNumbers[10][6] PROGMEM =
{
    {4, 0, 5, 4, 1, 5},         //0
    {0, BLACK, EMPTY, 1, BLACK, 1},     //1
    {0, 0, BLACK, BLACK, 2, 1},         //2
    {3, 3, 5, 1, 1, 5},         //3
    {4, 1, 5, EMPTY, EMPTY, 5}, //4
    {BLACK, 3, 0, 1, 1, BLACK},         //5
    {4, 0, 0, 4, 2, BLACK},         //6
    {0, 0, 5, EMPTY, EMPTY, 5},   //7
    {4, 0, 5, 4, 2, 5},         //8
    {BLACK, 3, 5, 1, 1, 5}         //9
};
bool anim_start_pos = false;
void printBigNum(LiquidCrystal_I2C* lcd, uint8_t number, uint8_t startCol, uint8_t startRow)
{
    lcd->setCursor(startCol, startRow);
    bool anim_pos = anim_start_pos;
    for (uint8_t cnt = 0; cnt < 6; cnt++)
    {
        lcd->write((random(0,2) || ntp_time_set()) ? (char)pgm_read_byte(&(BigNumbers[number][cnt])) : ' ');
        if (cnt == 2)
        {
            lcd->setCursor(startCol, startRow + 1);
        }
    }
    anim_start_pos = !anim_start_pos;
}

void set_dots(LiquidCrystal_I2C* lcd, bool state)
{
    lcd->setCursor(6, 0);
    lcd->write(state ? '.' : ' ');
    lcd->setCursor(6, 1);
    lcd->write(state ? '.' : ' ');
}


time_mode::time_mode()
{

}

time_mode::~time_mode()
{

}

bool time_mode::setup(LiquidCrystal_I2C* lcd, bool from_interaction)
{
    lcd->clear();

    for (int i = 0; i < 6; i++)
    {
        load_PGM_character_to(lcd, i, CustomChars[i]);
    }

    set_dots(lcd, dots_visibility);

    time_lastMin = 255;
    time_lastHr = 255;
    statusbar_set_mode(lcd, STATUSBAR_MODE_DEFAULT);
    return true;
}

void time_mode::update(LiquidCrystal_I2C* lcd)
{
    uint8_t new_hr = 88;
    uint8_t new_min = 88;
    if (ntp_time_set())
    {
        tm* ptr = localtime(ntp_get_epoch_time());
        new_hr = ptr->tm_hour;
        new_min = ptr->tm_min;
    }

    if (time_lastHr != new_hr || !ntp_time_set())
    {
        printBigNum(lcd, new_hr / 10, 0, 0);
        printBigNum(lcd, new_hr % 10, 3, 0);
        time_lastHr = new_hr;
    }

    if (time_lastMin != new_min || !ntp_time_set())
    {
        printBigNum(lcd, new_min / 10, 7, 0);
        printBigNum(lcd, new_min % 10, 10, 0);
        time_lastMin = new_min;
    }

    if (ntp_time_set())
    {
        dots_visibility = !dots_visibility;
    }
    else
        dots_visibility = true;

    set_dots(lcd, dots_visibility);

}

bool time_mode::can_skip()
{
    return true;
}

bool time_mode::on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len)
{
    return false;
}

uint32_t time_mode::update_time()
{
    return ntp_time_set() ? 1000 : 300;
}

uint32_t time_mode::show_time()
{
    return 50000;
}

