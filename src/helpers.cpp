#include "helpers.h"

char byte_to_char(uint8_t value)
{
    return (value < 10 ? 48 + value : 65 + (value - 10));
}

uint8_t* hex2bin(const char* str)
{
    int len, h;
    uint8_t* result, * p, c;

    if (!str)
        return NULL;

    if (!*str)
        return NULL;

    len = 0;
    p = (uint8_t*)str;
    while (*p++)
        len++;

    result = (uint8_t*)malloc((len / 2) + 1);
    h = !(len % 2) * 4;
    p = result;
    *p = 0;

    c = *str;
    while (c)
    {
        if (('0' <= c) && (c <= '9'))
            *p += (c - '0') << h;
        else if (('A' <= c) && (c <= 'F'))
            *p += (c - 'A' + 10) << h;
        else if (('a' <= c) && (c <= 'f'))
            *p += (c - 'a' + 10) << h;
        else
            return NULL;

        str++;
        c = *str;

        if (h)
            h = 0;
        else
        {
            h = 4;
            p++;
            *p = 0;
        }
    }

    return result;
}

void lcd_print_int(LiquidCrystal_I2C* lcd, uint32_t value)
{
    lcd->rightToLeft();
    if (value == 0)
        lcd->write('0');
    while (value != 0)
    {
        lcd->write(byte_to_char(value % 10));
        value = value / 10;
    }
    lcd->leftToRight();
}

char* devname_buffer;

const char* get_device_name()
{
    return devname_buffer;
}

void make_device_name()
{
    devname_buffer = new char[20];
    uint8_t len = sprintf(devname_buffer, "%s-%x", DEVICE_NAME_PREFIX, ESP.getChipId());
    //devname_buffer[len] = '\0';
}

uint8_t get_percent_from_RSSI(int8_t rssi)
{
    if (rssi <= -100)
    {
        return 0;
    }
    else if (rssi >= -50)
    {
        return 100;
    }
    else
    {
        return 2 * (rssi + 100);
    }
}

void load_PGM_character_to(LiquidCrystal_I2C* lcd, uint8_t place, const uint8_t* character)
{
    uint8_t tmp[8];
    memcpy_P(tmp, character, 8);
    lcd->createChar(place, tmp);
}