#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#include "helpers.h"
#include "display/display.h"
#include "button/button.h"
#include "wifi/wifi.h"
#include "ntp/ntp.h"
#include "ota/ota.h"
#include "mqtt/mqtt.h"
#include "message_from_mqtt.h"
#include "display/appearance/lock.h"
#include "display/modes/weather_mode/weather_mode.h"
#include "mqtt/homeassistant_autodiscovery/hass_mqtt.h"

bool safe_start = false;
weather_mode* display_weather_mode;

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

	display_weather_mode = new weather_mode();
	display_add_mode(display_weather_mode);

	if (safe_start)
	{
		display_show_mode(new message_mode(PSTR("Safe mode"), PSTR("loaded"), NULL, 2000, false));
		display_update();
		WiFi.begin(WIFI_SSID, WIFI_KEY);
		ArduinoOTA.begin();
		return;
	}

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
	button_update();
	wifi_update();
	ntp_update();
	ota_update();
	mqtt_loop();
	send_sensors();
}

void button_callback(const char* pattern, uint8_t pattern_len, uint32_t press_time)
{
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
	case wifi_event::WIFI_ON_CONNECTED:
		mqtt_connect(MQTT_BROKER_HOSTNAME, MQTT_BROKER_PORT, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD);
		break;
	}
}

void mqtt_on_connected(AsyncMqttClient* client)
{
	autodiscovery_send(client);
	subscribe_to_lock(client);
	subscribe_to_message(client);
	display_weather_mode->mqtt_subscribe(client);
}

void mqtt_on_message(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
	on_lock_message(topic, payload, len);
	check_message(topic, payload, len);
	display_weather_mode->on_mqtt_data(topic, payload, len);
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
