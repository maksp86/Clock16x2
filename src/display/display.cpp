#include "display.h"

LiquidCrystal_I2C lcd(0x27, 16, 2, LCD_SDA, LCD_SCL);

DisplayMode* display_modes[10];
uint8_t display_persistent_modes_cnt = 0;
uint8_t display_modes_cnt;
uint8_t display_curr_mode = 0;
#define curr_mode display_modes[display_curr_mode]
uint32_t display_mode_update_timer;
uint32_t display_mode_change_timer;

void display_skip_mode(bool from_interaction)
{
    if (display_modes_cnt == 1)
        return;


#if DEBUG >= 3
    Serial.println("display_skip_mode()");
#endif

    if (display_modes_cnt == display_persistent_modes_cnt && get_lock_state())
    {
        if (display_curr_mode != 0)
            display_curr_mode = 0;
        else
            return;
    }
    else
    {
        if (display_curr_mode >= display_persistent_modes_cnt)
        {
#if DEBUG >= 3
            Serial.println("display_curr_mode >= display_persistent_modes_cnt");
#endif
            delete curr_mode;
            curr_mode = NULL;
            if ((display_modes_cnt - display_persistent_modes_cnt) > 1)
            {
                for (uint8_t i = 0; i < (display_modes_cnt - display_persistent_modes_cnt) - 1; i++)
                {
                    display_modes[display_persistent_modes_cnt + i] = display_modes[display_persistent_modes_cnt + i + 1];
                }
            }
            display_modes_cnt--;
        }
        else
        {
            display_curr_mode++;
        }

        if (display_curr_mode >= display_modes_cnt)
        {
            display_curr_mode = 0;
        }
    }
    scrolling_text_destroy(true);

    if (!curr_mode->setup(&lcd, from_interaction))
    {
#if DEBUG >= 3
        Serial.printf("display_curr_mode %d setup fail\n", display_curr_mode);
#endif
        display_skip_mode(false);
        return;
    }

    display_mode_change_timer = millis();

    statusbar_full_redraw(&lcd);
    curr_mode->update(&lcd);

#if DEBUG >= 3
    Serial.printf("\tnew mode %d\n", display_curr_mode);
#endif
}

void display_setup()
{
#if DEBUG >= 3
    Serial.println("display_setup()");
#endif
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.backlight();
    lcd.write_P(PSTR("Booting"));

    //set persistent modes
    display_modes[display_persistent_modes_cnt++] = new time_mode();

#ifdef HTU21D_ENABLED
    display_modes[display_persistent_modes_cnt++] = new local_climate_mode();
#endif
    //set persistent modes

    display_modes_cnt = display_persistent_modes_cnt;
    curr_mode->setup(&lcd, false);

    backlight_setup(&lcd);
}

void display_update()
{
    if (display_modes_cnt > 0)
    {
        if (millis() - display_mode_update_timer > curr_mode->update_time())
        {
            // #if DEBUG >= 3
            //             Serial.printf("\tupdate mode %d\n", display_curr_mode);
            // #endif
            curr_mode->update(&lcd);
            display_mode_update_timer = millis();
        }

        if (curr_mode->can_skip() && millis() - display_mode_change_timer > curr_mode->show_time())
        {
#if DEBUG >= 3
            Serial.printf("\tskip mode %d\n", display_curr_mode);
#endif
            display_skip_mode(false);
            display_mode_change_timer = millis();
        }

    }
    statusbar_update(&lcd);
    backlight_update(&lcd);
}

bool display_show_mode(DisplayMode* mode)
{
#if DEBUG >= 3
    Serial.print("display_show_mode ");
#endif

    if (!mode->setup(&lcd, false))
        return false;

    if (display_modes_cnt > display_persistent_modes_cnt)
    {
#if DEBUG >= 3
        Serial.print("deleting temp modes");
#endif
        for (uint8_t i = 0; i < (display_modes_cnt - display_persistent_modes_cnt); i++)
        {
            delete display_modes[i + display_persistent_modes_cnt];
        }
        display_modes_cnt = display_persistent_modes_cnt;
    }
    display_curr_mode = display_modes_cnt++;
    curr_mode = mode;
    curr_mode->update(&lcd);
    display_mode_change_timer = millis();

#if DEBUG >= 3
    Serial.println();
#endif
    return true;
}

bool display_add_mode(DisplayMode* mode)
{
    if (display_modes_cnt == 10)
        return false;
    display_modes[display_persistent_modes_cnt++] = mode;
    display_modes_cnt = display_persistent_modes_cnt;
    return true;
}

bool display_enqueue_mode(DisplayMode* mode)
{
#if DEBUG >= 3
    Serial.print("display_enqueue_mode ");
#endif
    if (display_modes_cnt == 10)
        return false;

    if (!mode->setup(&lcd, false))
        return false;

    if (display_modes_cnt == display_persistent_modes_cnt)
        display_curr_mode = display_modes_cnt;

    display_modes[display_modes_cnt++] = mode;
    curr_mode->update(&lcd);
    display_mode_change_timer = millis();
#if DEBUG >= 3
    Serial.println();
#endif
    return true;
}

void display_on_interact(const char* pattern, uint8_t pattern_len)
{
#if DEBUG >= 3
    Serial.println("display_on_interact()");
#endif

    if (get_lock_state())
        return;

    bool is_single_click = strncmp(pattern, "s", pattern_len) == 0;

    if (!backlight_get_state() && is_single_click)
    {
        backlight_night_mode_arm(&lcd);
        return;
    }

    if (curr_mode->on_interact(&lcd, pattern, pattern_len))
    {
        return;
    }

#if DEBUG >= 3
    Serial.print("click pattern: ");
    Serial.write(pattern, pattern_len);
    Serial.println();
#endif

    if (is_single_click)
    {
        display_skip_mode(true);
    }

}