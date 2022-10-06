#include "button.h"

char click_pattern[MAX_PATTERN_LEN + 1];
uint32_t button_last_press_time, button_timer;
bool button_state;
uint8_t patternActions;

void button_setup()
{

}

void button_update()
{
    uint32_t currTime = millis();
    if (currTime - button_timer > 50)
    {
        uint32_t pressTime = currTime - button_last_press_time;

        if (digitalRead(BUTTON_PIN))
        {
            if (!button_state)
            {
                button_last_press_time = currTime;
                button_state = true;
            }
            else
            {
                if (pressTime > 8000)
                {
                    button_last_press_time = currTime;
                }
            }
        }
        else
        {
            if (button_state)
            {
                button_state = false;
                button_last_press_time = currTime;
                if (patternActions < MAX_PATTERN_LEN)
                {
                    click_pattern[patternActions] = (pressTime > 1000) ? 'l' : 's';

                    patternActions++;
                    return;
                }
            }

            if (pressTime > 250 && patternActions > 0)
            {
                click_pattern[patternActions] = '\0';
                button_callback(click_pattern, patternActions, pressTime);
                button_state = false;
                patternActions = 0;
            }

        }
        button_timer = millis();
    }
}
