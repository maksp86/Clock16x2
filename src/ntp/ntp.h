#ifndef __NTP_H__
#define __NTP_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncUDP.h>

#include "inttypes.h"
#include "wifi/wifi.h"

extern "C"
{
#include "lwip/init.h"
#include "lwip/dns.h"
#include "lwip/err.h"
}

#define NTP_PACKET_TIMEOUT_MS 1000
#define NTP_PACKET_SIZE 48
#define NTP_REQUEST_STATE_NONE -1
#define NTP_REQUEST_STATE_DNS_REQ 0
#define NTP_REQUEST_STATE_UDP_SEND 1
#define NTP_REQUEST_STATE_UDP_RECV 2

void ntp_setup(const char* ntp_server = "pool.ntp.org", int8_t hour_offset = 0, uint32_t update_time = 60000);
void ntp_update();
void ntp_on_wifi_event(wifi_event event);
time_t* ntp_get_epoch_time();
bool ntp_time_set();
#endif // __NTP_H__