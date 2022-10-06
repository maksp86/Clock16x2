#include "message_mode.h"

message_mode::message_mode(const char* _title, uint8_t _title_len, const char* _message, uint8_t _message_len, const uint8_t* _icon, uint32_t show_time, bool can_skip) :
    title_len(_title_len),
    message_len(_message_len),
    _show_time(show_time),
    _can_skip(can_skip)
{
#if DEBUG >= 4
    Serial.print("message_mode::message_mode ");
#endif
    if (_title != NULL)
    {
#if DEBUG >= 4
        Serial.print((_title));
#endif
        title = new char[title_len + 1];
        strcpy(title, _title);
        title[title_len] = '\0';
    }
    else
    {
        title = NULL;
        title_len = 0;
    }
    if (_message != NULL)
    {
#if DEBUG >= 4
        Serial.print((_message));
#endif
        message = new char[message_len + 1];
        strcpy(message, _message);
        message[message_len] = '\0';
    }
    else
    {
        message = NULL;
        message_len = 0;
    }

    if (_icon != NULL)
    {
        icon_arr = new uint8_t[ICON_SIZE];
        memcpy(icon_arr, _icon, ICON_SIZE);
    }
    else
    {
        icon_arr = NULL;
    }
#if DEBUG >= 4
    Serial.println();
#endif
}

message_mode::message_mode(PGM_P _title, PGM_P _message, const byte** _icon, uint32_t show_time, bool can_skip) :
    _show_time(show_time),
    _can_skip(can_skip)
{
#if DEBUG >= 4
    Serial.print("message_mode::message_mode ");
#endif
    if (_title != NULL)
    {
#if DEBUG >= 4
        Serial.print(FPSTR(_title));
#endif
        title_len = strlen_P(_title);
        title = new char[title_len + 1];
        strcpy_P(title, _title);
        title[title_len] = '\0';
    }
    else
    {
        title = NULL;
        title_len = 0;
    }
    if (_message != NULL)
    {
#if DEBUG >= 4
        Serial.print(FPSTR(_message));
#endif
        message_len = strlen_P(_message);
        message = new char[message_len + 1];
        strcpy_P(message, _message);
        message[message_len] = '\0';
    }
    else
    {
        message = NULL;
        message_len = 0;
    }

    if (_icon != NULL)
    {
        icon_arr = new uint8_t[ICON_SIZE];
        for (uint8_t i = 0; i < 4; i++)
        {
            memcpy_P(icon_arr + (i * 8), _icon[i], 8);
        }
    }
    else
    {
        icon_arr = NULL;
    }
#if DEBUG >= 4
    Serial.print("\n");
#endif
}

message_mode::~message_mode()
{
    if (title != NULL)
        delete[] title;
    if (message != NULL)
        delete[] message;
    if (icon_arr != NULL)
        delete[] icon_arr;
}

bool message_mode::setup(LiquidCrystal_I2C* lcd, bool from_interaction)
{
    lcd->clear();

    if (icon_arr != NULL)
    {
        load_PGM_character_to(lcd, 0, icon_arr);
        load_PGM_character_to(lcd, 1, icon_arr + 8);
        load_PGM_character_to(lcd, 2, icon_arr + 16);
        load_PGM_character_to(lcd, 3, icon_arr + 24);
        lcd->setCursor(0, 0);
        lcd->write((uint8_t)0);

        lcd->setCursor(1, 0);
        lcd->write((uint8_t)1);

        lcd->setCursor(0, 1);
        lcd->write((uint8_t)2);

        lcd->setCursor(1, 1);
        lcd->write((uint8_t)3);
    }

    uint8_t max_len = (icon_arr != NULL) ? 13 : 15;
    if (title != NULL)
    {
        lcd->setCursor((icon_arr != NULL) ? 2 : 0, 0);
        lcd->write((uint8_t*)title, (title_len < max_len) ? title_len : max_len);
    }

    if (message != NULL)
    {

        if ((icon_arr != NULL && message_len > 13) || (icon_arr == NULL && message_len > 15))
        {
            scrolling_text_prepare(message, message_len, 7, (icon_arr != NULL) ? 2 : 0, 1, (icon_arr != NULL) ? 13 : 15);
        }
        else
        {
            lcd->setCursor((icon_arr != NULL) ? 2 : 0, 1);
            lcd->write((uint8_t*)message, (message_len < max_len) ? message_len : max_len);
        }
    }

    statusbar_set_mode(lcd, STATUSBAR_MODE_SLIM);
    return true;
}

void message_mode::update(LiquidCrystal_I2C* lcd)
{
    if ((icon_arr != NULL && message_len > 13) || (icon_arr == NULL && message_len > 15))
    {
        scrolling_text_loop(lcd);
    }
}

bool message_mode::can_skip()
{
    return _can_skip;
}

bool message_mode::on_interact(LiquidCrystal_I2C* lcd, const char* pattern, uint8_t pattern_len)
{
    return !_can_skip;
}

uint32_t message_mode::update_time()
{
    return 300;
}

uint32_t message_mode::show_time()
{
    return _show_time;
}

