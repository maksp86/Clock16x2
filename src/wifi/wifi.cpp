#include "wifi.h"

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
uint8_t wifi_timeout_timer_count;
void wifi_arm_timer();
void wifi_disarm_timer();

bool need_wifi_suppress_events;

void _wifi_try_connect()
{
#if DEBUG >= 2
    Serial.printf("status %d\n", WiFi.status());
#endif

#if DEBUG >= 2
    Serial.printf("connecting to %s\n", WiFi.SSID().c_str());
#endif
    WiFi.begin(WIFI_SSID, WIFI_KEY);

    wifi_last_status = wifi_status::WIFI_CONNECTING;
    wifi_on_event(wifi_event::WIFI_ON_CONNECTING);
}

void _wifi_handle_timeout()
{
    wifi_last_status = wifi_status::WIFI_IDLE_STATUS;
    wifi_on_event(wifi_event::WIFI_ON_CONNECT_TIMEOUT);
    //TODO
}

void wifi_setup()
{
    need_wifi_suppress_events = false;

    onmodechanged = WiFi.onWiFiModeChange(&wifi_on_mode_change);

    onconnected = WiFi.onStationModeConnected(&wifi_on_connected);
    ondisconnected = WiFi.onStationModeDisconnected(&wifi_on_disconnected);

    ondhcpdone = WiFi.onStationModeGotIP(&wifi_got_ip);
    ondhcptimeout = WiFi.onStationModeDHCPTimeout(&wifi_got_ip_timeout);

    WiFi.setHostname(get_device_name());
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);
    WiFi.mode(WiFiMode_t::WIFI_STA);

    wifi_disarm_timer();
}

void wifi_update()
{
    if (!_wifi_setupped)
    {
        _wifi_try_connect();
        wifi_arm_timer();
        _wifi_setupped = true;
    }
    else
    {
        if (need_wifi_timeout_timer && millis() - wifi_timeout_timer > 30000)
        {
            if (wifi_timeout_timer_count < 3)
            {
                _wifi_try_connect();
                wifi_timeout_timer = millis();
            }
            else
            {
                wifi_on_event(wifi_event::WIFI_ON_CONNECT_TIMEOUT);
                _wifi_handle_timeout();
                wifi_disarm_timer();
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
    if (arg.oldMode == WiFiMode_t::WIFI_STA && arg.newMode == WiFiMode_t::WIFI_AP)
    {
        wifi_last_status = wifi_status::WIFI_AP_ENABLED;
        wifi_on_event(wifi_event::WIFI_ON_AP_START);
    }

    if (arg.oldMode == WiFiMode_t::WIFI_AP && arg.newMode == WiFiMode_t::WIFI_STA)
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