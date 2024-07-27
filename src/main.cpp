#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "helpers.h"
#include "config/config.h"
#include "display/display.h"

#ifdef HTU21D_ENABLED
#include "htu21d/htu21d.h"
#endif

#include "button/button.h"
#include "wifi/wifi.h"
#ifdef USE_WEBSERVER
#include "webserver/webserver.h"
#endif
#include "ntp/ntp.h"
#include "ota/ota.h"
#include "mqtt/mqtt.h"
#include "message_from_mqtt.h"

#ifdef USE_HOMEASSISTANT
#include "display/modes/weather_mode/weather_mode.h"
#include "display/appearance/lock.h"
#include "mqtt/homeassistant_autodiscovery/hass_mqtt.h"
weather_mode* display_weather_mode;
#endif

bool safe_start = false;

void setup()
{
	ESP.wdtDisable();
	ESP.wdtEnable(WDTO_2S);
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

	config_setup();
	make_device_name();
	display_setup();

#ifdef USE_HOMEASSISTANT
	display_weather_mode = new weather_mode();
	display_add_mode(display_weather_mode);
#endif

	if (safe_start)
	{
		display_show_mode(new message_mode(PSTR("Safe mode loaded"), PSTR("ap started"), NULL, 2000, false));
		display_update();
		WiFi.mode(WiFiMode_t::WIFI_AP);
		WiFi.softAP(get_device_name());
		ArduinoOTA.begin();
		return;
	}

#ifdef HTU21D_ENABLED
	htu21d_setup();
#endif

	button_setup();
	wifi_setup();
	ntp_setup();

	ota_setup();
	mqtt_setup();
#ifdef USE_WEBSERVER
	webserver_setup();
#endif

#if DEBUG >= 1
	Serial.printf("%s successfully booted", get_device_name());
#endif
}

#if DEBUG >= 5
uint32_t memTestTimer = 0;
#endif

void loop()
{
	if (is_need_restart())
	{
		delay(1000);
		wifi_suppress_events();
		ESP.restart();
		delay(5000);
	}

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
#ifdef USE_WEBSERVER
	webserver_loop();
#endif
	config_loop();

#ifdef USE_HOMEASSISTANT
	send_sensors();
#endif

#if DEBUG >= 5
	if (millis() - memTestTimer > 2000)
	{
		Serial.printf("Free mem: %u\n", ESP.getFreeHeap());
		memTestTimer = millis();
	}
#endif

}

void button_callback(const char* pattern, uint8_t pattern_len, uint32_t press_time)
{
	if (strcmp(pattern, ("sssss")) == 0)
	{
		set_need_restart();
	}
	else
		display_on_interact(pattern, pattern_len);
}

void wifi_on_event(wifi_event event)
{
	ntp_on_wifi_event(event);
#ifdef USE_WEBSERVER
	webserver_wifi_on_event(event);
#endif
#if DEBUG >= 2
	Serial.printf("wifi_on_event() %d\n", (uint8_t)event);
#endif

	switch (event)
	{
	case wifi_event::WIFI_ON_CONNECT_TIMEOUT:
#if DEBUG >= 1
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("timeout"), NULL, 1000, true));
#endif
		break;

	case wifi_event::WIFI_ON_DISCONNECT:
#if DEBUG >= 1
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("disconnect"), NULL, 1000, true));
#endif
		mqtt_disconnect(true);
		break;

	case wifi_event::WIFI_ON_CONNECTING:
#if DEBUG >= 1
		display_show_mode(new message_mode(PSTR("wifi_event"), PSTR("connecting"), NULL, 1000, true));
#endif
		statusbar_set_wifi_busy();
		break;

	case wifi_event::WIFI_ON_CONNECTED:
		display_show_mode(new message_mode(PSTR("WiFi"), (F("connected to ") + WiFi.SSID()).c_str(), NULL, 5000, true));
		mqtt_connect();
		break;

	case wifi_event::WIFI_ON_AP_START:
		display_show_mode(new message_mode(PSTR("AP started"), (WiFi.softAPSSID() + String("/") + WiFi.softAPPSK()).c_str(), NULL, 1000, false));
		break;
	}
}

void mqtt_on_connected(AsyncMqttClient* client)
{
#if DEBUG >= 4
	Serial.println("mqtt_on_connected");
#endif
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
	on_message_mqtt(topic, payload, len);

#ifdef USE_HOMEASSISTANT
	on_lock_message(topic, payload, len);
	display_weather_mode->on_mqtt_data(topic, payload, len);
#endif

	// caused wdt reset
#if DEBUG >= 4
	Serial.println("Publish received.");
	Serial.print("  topic: ");
	Serial.println(topic);
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
	Serial.print("  payload: ");
	Serial.write(payload, len);
	Serial.println();
#endif
}
