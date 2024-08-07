#include "ota/ota.h"

void ota_on_start();
void ota_on_end();
void ota_on_progress(unsigned int progress, unsigned int total);
void ota_on_error(ota_error_t error);

progress_mode* ota_progress_mode = NULL;
bool ota_progress_mode_init = false;

void ota_setup()
{
    ArduinoOTA.setHostname(get_device_name());
    ArduinoOTA.onStart(&ota_on_start);
    ArduinoOTA.onEnd(&ota_on_end);
    ArduinoOTA.onProgress(&ota_on_progress);
    ArduinoOTA.onError(&ota_on_error);

    char* ota_pass;
    if (config_get_str("ota_pass", &ota_pass))
    {
        if (strcmp(ota_pass, "") != 0)
            ArduinoOTA.setPassword(ota_pass);
        delete[] ota_pass;
    }


    ArduinoOTA.begin(true);
}

void ota_update()
{
    ArduinoOTA.handle();
}

void ota_on_start()
{
#if DEBUG >= 2
    Serial.println("ota_on_start");
#endif
    message_mode* mode = new message_mode(PSTR("OTA starting"), NULL, NULL, 2000, true);
    display_show_mode(mode);
    display_update();
}

void ota_on_end()
{
#if DEBUG >= 2
    Serial.println("ota_on_end");
#endif
    wifi_suppress_events();
    ota_progress_mode_init = false;
    message_mode* mode = new message_mode(PSTR("OTA complete"), PSTR("rebooting"), NULL, 2000, true);
    display_show_mode(mode);
    display_update();
}

void ota_on_progress(unsigned int progress, unsigned int total)
{
#if DEBUG >= 2
    Serial.printf("ota_on_progress %u/%u\r", progress, total);
#endif
    if (!ota_progress_mode_init)
    {
        ota_progress_mode = new progress_mode(PSTR("OTA progress"), false, 0, total);
        display_show_mode(ota_progress_mode);
        ota_progress_mode_init = true;
    }
    ota_progress_mode->set_progress(progress);
    statusbar_set_wifi_busy();
    display_update();
}

void ota_on_error(ota_error_t error)
{
#if DEBUG >= 2
    Serial.print("ota_on_error ");
#endif
    ota_progress_mode_init = false;
    PGM_P message = NULL;
    switch (error)
    {
    case ota_error_t::OTA_AUTH_ERROR:
        message = PSTR("auth error");
        break;
    case ota_error_t::OTA_BEGIN_ERROR:
        message = PSTR("begin error");
        break;
    case ota_error_t::OTA_CONNECT_ERROR:
        message = PSTR("connect error");
        break;
    case ota_error_t::OTA_RECEIVE_ERROR:
        message = PSTR("receive error");
        break;
    case ota_error_t::OTA_END_ERROR:
        message = PSTR("end error");
        break;
    }
#if DEBUG >= 2
    Serial.println(FPSTR(message));
#endif
    message_mode* mode = new message_mode(PSTR("OTA failed"), message, NULL, 2000, true);
    display_show_mode(mode);
    display_update();
}