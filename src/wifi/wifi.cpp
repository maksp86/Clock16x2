#include "wifi.h"

char* wifi_ssid = nullptr;
char* wifi_pass = nullptr;

bool _wifi_setupped;
wifi_status wifi_last_status = wifi_status::WIFI_IDLE_STATUS;
//WiFiManager* wifi_manager;

WiFiEventHandler onmodechanged;
void wifi_on_mode_change(const WiFiEventModeChange& arg);

WiFiEventHandler onconnected, ondisconnected;
void wifi_on_connected(const WiFiEventStationModeConnected& arg);
void wifi_on_disconnected(const WiFiEventStationModeDisconnected& arg);

WiFiEventHandler ondhcpdone, ondhcptimeout;
void wifi_got_ip(const WiFiEventStationModeGotIP& arg);
void wifi_got_ip_timeout();

bool need_wifi_timeout_timer;
uint32_t wifi_timeout_timer;
uint8_t wifi_timeout_timer_count = 0;
void wifi_arm_timer();
void wifi_disarm_timer();

bool need_wifi_suppress_events;

void _wifi_try_connect()
{
#if DEBUG >= 2
    Serial.printf("status %d\n", WiFi.status());
#endif

    WiFi.begin(wifi_ssid, wifi_pass);
#if DEBUG >= 2
    Serial.printf("connecting to %s\n", wifi_ssid);
#endif

    wifi_last_status = wifi_status::WIFI_CONNECTING;
    wifi_on_event(wifi_event::WIFI_ON_CONNECTING);
}

void _wifi_handle_timeout()
{
#if DEBUG >= 2
    Serial.println("_wifi_handle_timeout");
#endif
    wifi_last_status = wifi_status::WIFI_IDLE_STATUS;
    wifi_on_event(wifi_event::WIFI_ON_CONNECT_TIMEOUT);
    wifi_start_ap();
    //TODO
}

void wifi_setup()
{
    need_wifi_suppress_events = false;

    bool configured = config_get_str("wifi_ssid", &wifi_ssid);
    configured = configured && config_get_str("wifi_pass", &wifi_pass);

    onmodechanged = WiFi.onWiFiModeChange(&wifi_on_mode_change);

    onconnected = WiFi.onStationModeConnected(&wifi_on_connected);
    ondisconnected = WiFi.onStationModeDisconnected(&wifi_on_disconnected);

    ondhcpdone = WiFi.onStationModeGotIP(&wifi_got_ip);
    ondhcptimeout = WiFi.onStationModeDHCPTimeout(&wifi_got_ip_timeout);

    WiFi.hostname(get_device_name());
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);

    if (configured)
    {
        WiFi.mode(WiFiMode_t::WIFI_STA);
        wifi_disarm_timer();
    }
    else
    {
        wifi_start_ap();
    }
}

void wifi_update()
{
    if (WiFi.getMode() == WiFiMode_t::WIFI_STA)
    {
        if (!_wifi_setupped)
        {
            _wifi_try_connect();
            wifi_arm_timer();
            _wifi_setupped = true;
        }
        else
        {
            if (need_wifi_timeout_timer && millis() - wifi_timeout_timer > 15000)
            {
                if (wifi_timeout_timer_count < 3)
                {
                    _wifi_try_connect();
                    wifi_timeout_timer_count++;
                    wifi_timeout_timer = millis();
                }
                else
                {
                    _wifi_handle_timeout();
                    wifi_disarm_timer();
                }
            }
        }
    }
}

void wifi_arm_timer()
{
    if (!need_wifi_timeout_timer)
    {
#if DEBUG >= 2
        Serial.println("timer armed");
#endif
        wifi_timeout_timer = millis();
        need_wifi_timeout_timer = true;
    }
}

void wifi_disarm_timer()
{
#if DEBUG >= 2
    Serial.println("timer disarmed");
#endif
    need_wifi_timeout_timer = false;
    wifi_timeout_timer_count = 0;
}

wifi_status wifi_get_last_status()
{
    return wifi_last_status;
}

void wifi_on_mode_change(const WiFiEventModeChange& arg)
{
    if (need_wifi_suppress_events) return;
#if DEBUG >= 2
    Serial.printf("wifi_on_mode_change() %d %d \n", arg.oldMode, arg.newMode);
#endif
    if (wifi_last_status != wifi_status::WIFI_AP_ENABLED && arg.newMode == WiFiMode_t::WIFI_AP)
    {
        wifi_last_status = wifi_status::WIFI_AP_ENABLED;
        wifi_on_event(wifi_event::WIFI_ON_AP_START);
    }

    if (wifi_last_status == wifi_status::WIFI_AP_ENABLED && arg.newMode == WiFiMode_t::WIFI_STA)
    {
        wifi_last_status = wifi_status::WIFI_CONNECTING;
        wifi_on_event(wifi_event::WIFI_ON_AP_END);
    }
}

void wifi_on_connected(const WiFiEventStationModeConnected& arg)
{
    if (need_wifi_suppress_events) return;
#if DEBUG >= 2
    Serial.println("wifi_on_connected()");
#endif
    wifi_disarm_timer();
}

void wifi_on_disconnected(const WiFiEventStationModeDisconnected& arg)
{
    if (need_wifi_suppress_events) return;
#if DEBUG >= 2
    Serial.println("wifi_on_disconnected()");
#endif
    if (wifi_last_status == wifi_status::WIFI_CONNECTED)
    {
        wifi_last_status = wifi_status::WIFI_IDLE_STATUS;
        wifi_on_event(wifi_event::WIFI_ON_DISCONNECT);
        _wifi_try_connect();
        wifi_arm_timer();
    }
}

void wifi_got_ip(const WiFiEventStationModeGotIP& arg)
{
    if (need_wifi_suppress_events) return;
#if DEBUG >= 2
    Serial.println("wifi_got_ip()");
#endif
    if (wifi_last_status != wifi_status::WIFI_CONNECTED)
    {
        wifi_last_status = wifi_status::WIFI_CONNECTED;
        wifi_on_event(wifi_event::WIFI_ON_CONNECTED);
    }

}

void wifi_got_ip_timeout()
{
    if (need_wifi_suppress_events) return;
#if DEBUG >= 2
    Serial.println("wifi_got_ip_timeout()");
#endif

    if (wifi_last_status != wifi_status::WIFI_CONNECTED)
    {
#if DEBUG >= 2
        Serial.println("connect timeout");
#endif
        wifi_on_event(wifi_event::WIFI_ON_CONNECT_TIMEOUT);
        _wifi_try_connect();
        wifi_arm_timer();
    }
}

void wifi_suppress_events()
{
    need_wifi_suppress_events = true;
    wifi_disarm_timer();
}



void wifi_start_ap()
{
    if (WiFi.getMode() == WiFiMode_t::WIFI_AP)
        return;

#if DEBUG >= 2
    Serial.println("wifi_start_ap");
#endif

    WiFi.disconnect();
    char* ap_pass = new char[9];
    generate_random_string(ap_pass, 8);
    ap_pass[8] = '\0';
    WiFi.softAP(get_device_name(), ap_pass);
    WiFi.mode(WiFiMode_t::WIFI_AP);

#if DEBUG >= 2
    Serial.printf("wifi_start_ap %s %s\n", get_device_name(), ap_pass);
#endif

    delete[] ap_pass;
}

void wifi_end_ap()
{
    if (WiFi.getMode() == WiFiMode_t::WIFI_STA)
        return;

#if DEBUG >= 2
    Serial.println("wifi_end_ap");
#endif

    if (wifi_last_status == wifi_status::WIFI_AP_ENABLED)
    {
        WiFi.softAPdisconnect();
        WiFi.mode(WiFiMode_t::WIFI_STA);
    }
}