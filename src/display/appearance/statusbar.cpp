#include "statusbar.h"
#include "statusbar_icons.h"

uint32_t statusbar_timer;
uint8_t statusbar_curr_mode = STATUSBAR_MODE_DEFAULT;
int8_t stausbar_last_signal_strength = 0;
uint8_t stausbar_last_lock_state = false;

bool wifi_is_busy = false;
bool last_wifi_is_busy = false;

void statusbar_full_redraw(LiquidCrystal_I2C* lcd)
{
    switch (statusbar_curr_mode)
    {
    case STATUSBAR_MODE_DEFAULT:
    {
        lcd->setCursor(14, 0);
        lcd->write('|');
        lcd->setCursor(14, 1);
        lcd->write('|');
        break;
    }
    case STATUSBAR_MODE_SLIM:
        break;
    }
    stausbar_last_signal_strength = 255;
    stausbar_last_lock_state = false;
    statusbar_timer = 0;
    statusbar_update(lcd);
}

void statusbar_set_mode(LiquidCrystal_I2C* lcd, uint8_t mode)
{
    statusbar_curr_mode = mode;
    statusbar_full_redraw(lcd);
}

void statusbar_set_wifi_busy()
{
    wifi_is_busy = true;
}

void statusbar_update(LiquidCrystal_I2C* lcd)
{
    if (millis() - statusbar_timer > 1000)
    {
        if (stausbar_last_lock_state != get_lock_state())
        {
            if (get_lock_state())
            {
                load_PGM_character_to(lcd, STATUSBAR_LOCK_ICON_POS, lock_state_icon);
                lcd->setCursor(15, 1);
                lcd->write((char)STATUSBAR_LOCK_ICON_POS);
            }
            else
            {
                lcd->setCursor(15, 1);
                lcd->write((char)254);
            }
            stausbar_last_lock_state = get_lock_state();
        }

        int8_t zeros = 8;
        const uint8_t* icon_ptr = wifi_state_icon;

        switch (wifi_get_last_status())
        {
        case wifi_status::WIFI_CONNECTED:
        {
            int8_t signalStrength = get_percent_from_RSSI(WiFi.RSSI());
            if (signalStrength >= 75)
                zeros = 0;
            else if (signalStrength >= 50)
                zeros = 2;
            else
                zeros = 4;
            break;
        }

        case wifi_status::WIFI_CONNECTING:
        {
            if (stausbar_last_signal_strength - 2 >= 0)
            {
                zeros = stausbar_last_signal_strength - 2;
            }
            else
                zeros = 8;

            break;
        }

        case wifi_status::WIFI_AP_ENABLED:
        {
            icon_ptr = wifi_ap_icon;
            zeros = 0;
            break;
        }
        }

        if (wifi_is_busy && wifi_is_busy == last_wifi_is_busy)
        {
            wifi_is_busy = false;
        }

        if (zeros != stausbar_last_signal_strength || wifi_is_busy != last_wifi_is_busy)
        {
            uint8_t tmp[8];

            //custom memcpy_P and memset with inverse icon when wifi is busy
            for (uint8_t i = 0; i < 8; i++)
            {
                tmp[i] = i < zeros ? 0 : pgm_read_byte(&icon_ptr[i]);
            }
            if (wifi_is_busy) tmp[7] = 0b00011111;
            last_wifi_is_busy = wifi_is_busy;
            // memcpy_P(tmp, wifi_state_icon, 8);
            //memset(tmp, 0, zeros);

            lcd->createChar(STATUSBAR_WIFI_ICON_POS, tmp);
            lcd->setCursor(15, 0);
            lcd->write((char)STATUSBAR_WIFI_ICON_POS);
            stausbar_last_signal_strength = zeros;
        }

        statusbar_timer = millis();
    }
}
