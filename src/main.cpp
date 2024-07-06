#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#include "helpers.h"
#include "display/display.h"

#ifdef HTU21D_ENABLED
#include "htu21d/htu21d.h"
#endif

#include "button/button.h"
#include "wifi/wifi.h"
#include "ntp/ntp.h"
#include "ota/ota.h"
#include "mqtt/mqtt.h"
#include "message_from_mqtt.h"
#include "display/modes/weather_mode/weather_mode.h"

#ifdef USE_HOMEASSISTANT
#include "display/appearance/lock.h"
#include "mqtt/homeassistant_autodiscovery/hass_mqtt.h"
weather_mode* display_weather_mode;
#endif

bool safe_start = false;

void setup()
{
#if DEBUG >= 1
	Serial.begin(74880);
#endif

	rst_info* rst = ESP.getResetInfoPtr();
	if (rst->reason == rst_reason::REASON_EXCEPTION_RST)
	{
#if DEBUG >= 1
		Serial.printf("Reboot from exception %u\n", rst->reason);
#endif
		safe_start = true;
	}
	else
	{
#if DEBUG >= 1
		Serial.printf("Reboot from %u\n", rst->reason);
#endif
		safe_start = false;
	}

	make_device_name();
	display_setup();

#ifdef USE_HOMEASSISTANT
	display_weather_mode = new weather_mode();
	display_add_mode(display_weather_mode);
#endif

	if (safe_start)
	{
		display_show_mode(new message_mode(PSTR("Safe mode"), PSTR("loaded"), NULL, 2000, false));
		display_update();
		WiFi.begin(WIFI_SSID, WIFI_KEY);
		ArduinoOTA.begin();
		return;
	}

#ifdef HTU21D_ENABLED
	htu21d_setup();
#endif

	button_setup();
	wifi_setup();
	ntp_setup(NTP_SERVER, NTP_TIME_DELTA);

	ota_setup();
	mqtt_setup();

#if DEBUG >= 1
	Serial.printf("%s successfully booted", get_device_name());
#endif
}

void loop()
{
	if (safe_start)
	{
		ArduinoOTA.handle();
		return;
	}
	display_update();

#ifdef HTU21D_ENABLED
	htu21d_update();
#endif

	button_update();
	wifi_update();
	ntp_update();
	ota_update();
	mqtt_update();

#ifdef USE_HOMEASSISTANT
	send_sensors();
#endif
}

void button_callback(const char* pattern, uint8_t pattern_len, uint32_t press_time)
{
	if (strcmp(pattern, ("sssss")) == 0)
	{
		ESP.restart();
	}
	else
		display_on_interact(pattern, pattern_len);
}

void wifi_on_event(wifi_event event)
{
#if DEBUG >= 2
	Serial.printf("wifi_on_event() %d\n", (uint8_t)event);
#endif
	ntp_on_wifi_event(event);

	switch (event)
	{
	case wifi_event::WIFI_ON_CONNECT_TIMEOUT:
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("timeout"), NULL, 1000, true));
		break;
	case wifi_event::WIFI_ON_DISCONNECT:
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("disconnect"), NULL, 1000, true));
		break;
	case wifi_event::WIFI_ON_CONNECTING:
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("connecting"), NULL, 1000, true));
		statusbar_set_wifi_busy();
		break;
	case wifi_event::WIFI_ON_CONNECTED:
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("connected"), NULL, 1000, true));
		mqtt_connect(MQTT_BROKER_HOSTNAME, MQTT_BROKER_PORT, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD);
		break;
	}
}

void mqtt_on_connected(AsyncMqttClient* client)
{
	statusbar_set_wifi_busy();
	subscribe_to_message(client);

#ifdef USE_HOMEASSISTANT
	autodiscovery_send(client);
	subscribe_to_lock(client);
	display_weather_mode->mqtt_subscribe(client);
#endif
}

void mqtt_on_message(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
	statusbar_set_wifi_busy();
	check_message(topic, payload, len);

#ifdef USE_HOMEASSISTANT
	on_lock_message(topic, payload, len);
	display_weather_mode->on_mqtt_data(topic, payload, len);
#endif

#if DEBUG >= 4
	Serial.println("Publish received.");
	Serial.print("  topic: ");
	Serial.println(topic);
	Serial.print("  payload: ");
	Serial.write(payload, total);
	Serial.println();
	Serial.print("  qos: ");
	Serial.println(properties.qos);
	Serial.print("  dup: ");
	Serial.println(properties.dup);
	Serial.print("  retain: ");
	Serial.println(properties.retain);
	Serial.print("  len: ");
	Serial.println(len);
	Serial.print("  index: ");
	Serial.println(index);
	Serial.print("  total: ");
	Serial.println(total);
#endif
}
