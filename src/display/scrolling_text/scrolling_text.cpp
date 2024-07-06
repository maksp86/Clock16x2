#include "scrolling_text.h"

#pragma region ScrollingText

uint8_t* scrollingTextContainer = NULL;
#define scrollingTextContainer_init_len 5
#define scrolling_text_len scrollingTextContainer[0]
#define scrolling_i scrollingTextContainer[1]

#define get_scrolling_dir bitRead(scrollingTextContainer[2], 7)
#define set_scrolling_dir(val) bitWrite(scrollingTextContainer[2], 7, val)
#define get_scrolling_speed (scrollingTextContainer[2] & 0b111)
#define get_scrolling_speed_cnt ((scrollingTextContainer[2] & 0b111000) >> 3)
#define set_scrolling_speed_cnt(val) scrollingTextContainer[2] = ((get_scrolling_dir << 7) | (((val) & 0b111) << 3) | (get_scrolling_speed))

#define scrolling_pos scrollingTextContainer[3]
#define scrolling_width scrollingTextContainer[4]
#define scrolling_text (scrollingTextContainer + 5)

void scrolling_text_destroy(bool full)
{
    if (scrollingTextContainer != NULL)
        delete[] scrollingTextContainer;
    scrollingTextContainer = NULL;
}

void scrolling_text_prepare_all(uint8_t textlen, uint8_t speed, uint8_t col, uint8_t row, uint8_t width)
{
    if (scrollingTextContainer != NULL)
        free(scrollingTextContainer);
    scrollingTextContainer = new uint8_t[scrollingTextContainer_init_len + textlen + 1];
    scrolling_text_len = textlen;
    scrolling_i = 0;
    speed = 7 - speed;
    scrollingTextContainer[2] = (1 << 7) | ((speed & 0b111) << 3) | (speed & 0b111);
    scrolling_pos = col << 4 | row;
    scrolling_width = width;
}

void scrolling_text_prepare_P(PGM_P text, uint8_t speed, uint8_t col, uint8_t row, uint8_t width)
{
    uint8_t textlen = strlen_P(text);
    scrolling_text_prepare_all(textlen, speed, col, row, width);

    memcpy_P(scrolling_text, text, textlen);
    scrolling_text[textlen] = '\0';

#if DEBUG >= 4
    Serial.print("(scrolling_text_prepare)\n");

    Serial.print("\tscrolling_text: ");
    Serial.print(((const char*)scrolling_text));

    Serial.print("\n\tscrolling_text_len: ");
    Serial.print(scrolling_text_len);

    Serial.print("\n\tparam_container: ");
    Serial.print(scrollingTextContainer[2]);

    Serial.print("\n\tscrolling_pos: ");
    Serial.print(scrolling_pos);

    Serial.print("\n\tscrolling_width: ");
    Serial.println(scrolling_width);
#endif

}

void scrolling_text_prepare(const char* text, uint8_t textlen, uint8_t speed, uint8_t col, uint8_t row, uint8_t width)
{
    scrolling_text_prepare_all(textlen, speed, col, row, width);
    memcpy(scrolling_text, text, textlen);
    scrolling_text[textlen] = '\0';

#if DEBUG >= 4
    Serial.print("(scrolling_text_prepare)\n");

    Serial.print("\tscrolling_text: ");
    Serial.print(((const char*)scrolling_text));

    Serial.print("\n\tscrolling_text_len: ");
    Serial.print(scrolling_text_len);

    Serial.print("\n\tparam_container: ");
    Serial.print(scrollingTextContainer[2]);

    Serial.print("\n\tscrolling_pos: ");
    Serial.print(scrolling_pos);

    Serial.print("\n\tscrolling_width: ");
    Serial.println(scrolling_width);
#endif

}

void scrolling_text_update(LiquidCrystal_I2C* lcd)
{
    if (scrollingTextContainer == NULL || scrolling_text_len < 16 || scrolling_width == 0)
        return;

    if (get_scrolling_speed_cnt < get_scrolling_speed)
    {
        set_scrolling_speed_cnt(get_scrolling_speed_cnt + 1);
        return;
    }
    set_scrolling_speed_cnt(0);

    lcd->setCursor(scrolling_pos >> 4, scrolling_pos & 0x0f);
    if (get_scrolling_dir)
    {
        if ((scrolling_text_len - 1 - scrolling_i - (scrolling_width - 1)) > 0)
            scrolling_i++;
        else
            set_scrolling_dir(false);
    }
    else
    {
        if (scrolling_i > 0)
            scrolling_i--;
        else
            set_scrolling_dir(true);
    }
    lcd->write((uint8_t*)(scrolling_text + scrolling_i), scrolling_width);
}