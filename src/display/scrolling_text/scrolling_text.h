#ifndef __SCROLLING_TEXT_H__
#define __SCROLLING_TEXT_H__

#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "defines.h"

void scrolling_text_destroy(bool full = false);

/**
 * @brief prepare scrolling text
 *
 * @param text text to scroll
 * @param textlen length of text
 * @param speed speed of scroll 0-7
 * @param col x pos of scrolling text
 * @param row y pos of scrolling text
 * @param width width of scrolling text
 */
void scrolling_text_prepare(const char* text, uint8_t textlen, uint8_t speed = 0, uint8_t col = 0, uint8_t row = 0, uint8_t width = 16);
void scrolling_text_prepare_P(PGM_P text, uint8_t speed = 0, uint8_t col = 0, uint8_t row = 0, uint8_t width = 16);
void scrolling_text_loop(LiquidCrystal_I2C* lcd);

#endif // __SCROLLING_TEXT_H__