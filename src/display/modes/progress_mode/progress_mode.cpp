#include "progress_mode.h"

#define EMPTY_CELL_POS 0
#define FULL_CELL_POS 1
#define FIRST_CELL_POS 2
#define LAST_CELL_POS 3
#define CURR_CELL_POS 4

const char ProgressCellChars[5][8] PROGMEM
{
{
B11111,
B00000,
B00000,
B00000,
B00000,
B00000,
B00000,
B11111
},
{
B11111,
B00000,
B11111,
B11111,
B11111,
B11111,
B00000,
B11111
},
{
B01111,
B10000,
B10000,
B10000,
B10000,
B10000,
B10000,
B01111
},
{
B11110,
B00001,
B00001,
B00001,
B00001,
B00001,
B00001,
B11110
},
{
B11111,
B00000,
B00000,
B00000,
B00000,
B00000,
B00000,
B11111
}
};

progress_mode::progress_mode(
    char* _title, uint8_t _title_len, bool _is_intermidate, unsigned int _min_value, unsigned int _max_value) :
    title_len(_title_len),
    is_intermidate(_is_intermidate),
    min_value(_min_value),
    max_value(_max_value)
{
    curr_cell = new uint8_t[8];
    title = new char[title_len + 1];
    strcpy(title, _title);
    title[title_len] = '\0';
}

progress_mode::progress_mode(
    PGM_P _title, bool _is_intermidate, unsigned int _min_value, unsigned int _max_value) :
    is_intermidate(_is_intermidate),
    min_value(_min_value),
    max_value(_max_value)
{
    curr_cell = new uint8_t[8];
    title_len = strlen_P(_title);
#if DEBUG >= 4
    Serial.printf("progress_mode::progress_mode title_len: %d\n", title_len);
#endif
    title = new char[title_len + 1];
    strcpy_P(title, _title);
    title[title_len] = '\0';
}

progress_mode::~progress_mode()
{
    delete[] title;
    delete[] curr_cell;
}

bool progress_mode::setup(LiquidCrystal_I2C* lcd, bool from_interaction)
{
    lcd->clear();

    // if (title_len <= 14)
    // {
    //     lcd->setCursor(0, 0);
    //     lcd->write(title);
    // }
    if (title_len > 14)
        scrolling_text_prepare(title, title_len, 7, 0, 0, 14);
    else
    {
        lcd->setCursor(0, 0);
        lcd->write(title);
    }

    lcd->setCursor(0, 1);
    lcd->write(FIRST_CELL_POS);
    for (uint8_t i = 0; i < 11; i++)
    {
        lcd->setCursor(i + 1, 1);
        lcd->write((uint8_t)4);
    }
    lcd->setCursor(13, 1);
    lcd->write(LAST_CELL_POS);

    statusbar_set_mode(lcd, STATUSBAR_MODE_DEFAULT);
    for (int i = 0; i < 5; i++)
    {
        lcd->createChar_P(i, ProgressCellChars[i]);
    }
    //
    value = 0;
    last_value = 255;

    return true;
}

void progress_mode::update(LiquidCrystal_I2C* lcd)
{
    if (is_intermidate)
        update_intermidate(lcd);
    else
        update_normal(lcd);

    if (title_len > 14)
        scrolling_text_update(lcd);
}

void progress_mode::set_progress(unsigned int _value)
{
    if (!is_intermidate)
        value = _value;
}

bool progress_mode::can_skip()
{
    return value == max_value;
}

bool progress_mode::on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len)
{
    return true;
}

uint32_t progress_mode::update_time()
{
    return 300;
}

uint32_t progress_mode::show_time()
{
    return 1;
}

void fill_cell(LiquidCrystal_I2C* lcd, uint8_t* cell, uint8_t char_pos, uint8_t offset, uint8_t pos)
{
    memcpy_P(cell, ProgressCellChars[char_pos], 8);
    for (uint8_t i = 0; i < 4; i++)
    {
        cell[i + 2] = 0b11111 << offset;
    }
    lcd->createChar(char_pos, cell);
}

void progress_mode::update_intermidate(LiquidCrystal_I2C* lcd)
{
    if (last_value)
    {
        value++;
        if (value == 13)
        {
            last_value = false;
        }
    }
    else
    {
        value--;
        if (value == 0)
        {
            last_value = true;
        }
    }

    for (uint8_t i = 0; i < 14; i++)
    {
        uint8_t char_pos = i == value ? FULL_CELL_POS : EMPTY_CELL_POS;
        if (i == 0 || i == 13)
        {
            char_pos = ((i == 0) ? FIRST_CELL_POS : LAST_CELL_POS);
            fill_cell(lcd, curr_cell, char_pos, value == i ? 0 : 5, char_pos);
        }
        lcd->setCursor(i, 1);
        lcd->write(char_pos);

    }
}

void progress_mode::update_normal(LiquidCrystal_I2C* lcd)
{
    uint8_t dots = map(value, min_value, max_value, 0, 70);
    if (last_value != dots)
    {
        uint8_t full_fill = dots / 5;

        uint8_t screen_pos = full_fill;
        uint8_t char_pos = CURR_CELL_POS;

        if (full_fill == 0)
        {
            char_pos = FIRST_CELL_POS;
        }
        else if (full_fill == 1)
        {
            fill_cell(lcd, curr_cell, FIRST_CELL_POS, 0, FIRST_CELL_POS);
        }
        else if (full_fill > 12)
        {
            screen_pos = 13;
            char_pos = LAST_CELL_POS;
        }
        uint8_t bitoffset = (5 - (dots - (full_fill * 5)));
        //Serial.printf("value:%d\t dots: %d\t full_fill: %d\t screen_pos: %d\t char_pos: %d\t bitoffset: %d\n", value, dots, full_fill, screen_pos, char_pos, bitoffset);

        fill_cell(lcd, curr_cell, char_pos, bitoffset, char_pos);
        lcd->setCursor(screen_pos, 1);
        lcd->write(char_pos);

        for (uint8_t i = 1; i < 13; i++)
        {
            lcd->setCursor(i, 1);
            if (i < screen_pos)
            {
                lcd->write(FULL_CELL_POS);
            }
            else if (i > screen_pos && i < 13)
            {
                lcd->write((uint8_t)EMPTY_CELL_POS);
            }
        }

        last_value = dots;
    }
}
