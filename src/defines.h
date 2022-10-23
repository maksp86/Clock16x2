#ifndef __DEFINES_H__
#define __DEFINES_H__

#define DEBUG 4
#define LCD_SDA 0
#define LCD_SCL 3
#define BUTTON_PIN 2

#define DEVICE_NAME_PREFIX "clock16x2"

#define WIFI_SSID ""
#define WIFI_KEY ""

#define NTP_SERVER "pool.ntp.org"
#define NTP_TIME_DELTA 0

#define MQTT_BROKER_HOSTNAME "192.168.1.1"
#define MQTT_BROKER_PORT 1883
#define MQTT_BROKER_USERNAME ""
#define MQTT_BROKER_PASSWORD ""
#define MQTT_TOPIC_PREFIX "homeassistant/device/%s/" //do not touch /%s/

//#define OTA_PASSWORD ""

#endif // __DEFINES_H__
