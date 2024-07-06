#ifndef __WIFI_H__
#define __WIFI_H__

#include "Arduino.h"

#include "helpers.h"

typedef enum {
    WIFI_IDLE_STATUS = 0,
    WIFI_CONNECTED = 1,
    WIFI_CONNECTING = 2,
    WIFI_AP_ENABLED = 3,
} wifi_status;

typedef enum {
    WIFI_ON_CONNECTED = 0,
    WIFI_ON_CONNECT_TIMEOUT = 1,
    WIFI_ON_AP_NOT_FOUND = 2,
    WIFI_ON_DISCONNECT = 3,
    WIFI_ON_AP_START = 4,
    WIFI_ON_AP_END = 5,
    WIFI_ON_AP_STA_CON = 6,
    WIFI_ON_AP_STA_DISCON = 7,
    WIFI_ON_CONNECTING = 8
} wifi_event;

void wifi_setup();
void wifi_update();
void wifi_on_event(wifi_event event);
void wifi_suppress_events();
wifi_status wifi_get_last_status();


//extern WiFiManager* wifi_manager;

#endif // __WIFI_H__