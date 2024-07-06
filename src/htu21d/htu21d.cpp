#include "htu21d.h"

#ifdef HTU21D_ENABLED

HTU21 htu21d;

uint32_t htu21d_update_timer = 0;

float htu21d_temp;
float htu21d_hum;
bool htu21d_connected;

void htu21d_setup()
{
    if (htu21d.begin())
    {
#if DEBUG >= 1
        Serial.println("htu21d connected")
#endif
            htu21d_connected = true;
        htu21d.read();
        htu21d_temp = htu21d.getTemperature();
        htu21d_hum = htu21d.getHumidity();
    }
    else
    {
#if DEBUG >= 1
        Serial.println("htu21d not found")
#endif
            htu21d_connected = false;
    }

}

void htu21d_update()
{
    if ((millis() - htu21d_update_timer >= 60000) && htu21d_connected)
    {
        htu21d.read();
        htu21d_temp = htu21d.getTemperature();
        htu21d_hum = htu21d.getHumidity();
    }
}

bool htu21d_is_connected() { return htu21d_connected; }

float htu21d_get_temperature()
{
    if (htu21d_connected) return htu21d_temp;
    else return -1;
}

float htu21d_get_humidity()
{
    if (htu21d_connected) return htu21d_hum;
    else return -1;
}
#endif