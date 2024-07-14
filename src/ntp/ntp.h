#ifndef __NTP_H__
#define __NTP_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>

#include "inttypes.h"
#include "config/config.h"
#include "wifi/wifi.h"

#include "display/appearance/statusbar.h"

extern "C"
{
#include "lwip/init.h"
#include "lwip/dns.h"
#include "lwip/err.h"
}

#define NTP_REMOTE_PORT 123
#define NTP_LOCAL_PORT 1234
#define NTP_PACKET_TIMEOUT_MS 10000
#define NTP_PACKET_SIZE 48
#define NTP_REQUEST_STATE_NONE -1
#define NTP_REQUEST_STATE_DNS_REQ 0
#define NTP_REQUEST_STATE_UDP_SEND 1
#define NTP_REQUEST_STATE_UDP_RECV 2

void ntp_setup(uint32_t update_time = 60000);
void ntp_update();
void ntp_on_wifi_event(wifi_event event);
time_t* ntp_get_epoch_time();
bool ntp_time_set();
#endif // __NTP_H__