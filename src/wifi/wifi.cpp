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
void wifi_arm_timer();
void wifi_disarm_timer();

void wifi_setup()
{
    onmodechanged = WiFi.onWiFiModeChange(&wifi_on_mode_change);

    onconnected = WiFi.onStationModeConnected(&wifi_on_connected);
    ondisconnected = WiFi.onStationModeDisconnected(&wifi_on_disconnected);

    ondhcpdone = WiFi.onStationModeGotIP(&wifi_got_ip);
    ondhcptimeout = WiFi.onStationModeDHCPTimeout(&wifi_got_ip_timeout);

    WiFi.setHostname(get_device_name());
    if (WiFi.getAutoConnect())
        WiFi.setAutoConnect(false);

    //wifi_manager = new WiFiManager();

    wifi_disarm_timer();
}

void wifi_update()
{
    if (!_wifi_setupped)
    {
        WiFi.setAutoReconnect(true);
        WiFi.mode(WiFiMode_t::WIFI_STA);
#if DEBUG >= 2
        Serial.printf("status %d\n", WiFi.status());
#endif

        if (WiFi.status() == wl_status_t::WL_CONNECTED)
        {
#if DEBUG >= 2
            Serial.printf("reconnecting to %s\n", WiFi.SSID().c_str());
#endif
            WiFi.reconnect();
        }
        else
        {
#if DEBUG >= 2
            Serial.printf("connecting to %s\n", WiFi.SSID().c_str());
#endif
            //WiFi.begin();
            WiFi.begin(WIFI_SSID, WIFI_KEY);
        }
        wifi_last_status = wifi_status::WIFI_CONNECTING;
        _wifi_setupped = true;

        wifi_arm_timer();
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
}

wifi_status wifi_get_last_status()
{
    return wifi_last_status;
}

void wifi_on_mode_change(const WiFiEventModeChange& arg)
{
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
#if DEBUG >= 2
    Serial.println("wifi_on_connected()");
#endif
    wifi_disarm_timer();
}

void wifi_on_disconnected(const WiFiEventStationModeDisconnected& arg)
{
#if DEBUG >= 2
    Serial.println("wifi_on_disconnected()");
#endif
    if (wifi_last_status == wifi_status::WIFI_CONNECTED)
    {
        wifi_last_status = wifi_status::WIFI_CONNECTING;
        wifi_on_event(wifi_event::WIFI_ON_DISCONNECT);
        wifi_arm_timer();
    }
}

void wifi_got_ip(const WiFiEventStationModeGotIP& arg)
{
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
#if DEBUG >= 2
    Serial.println("wifi_got_ip_timeout()");
#endif

    wifi_disarm_timer();
    if (wifi_last_status != wifi_status::WIFI_CONNECTED)
    {
#if DEBUG >= 2
        Serial.println("connect timeout");
#endif
        WiFi.setAutoReconnect(false);
        wifi_on_event(wifi_event::WIFI_ON_CONNECT_TIMEOUT);
    }
}