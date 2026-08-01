#include <Arduino.h>

#include <WiFi.h>

#include "Config.h"
#include "CoreUtils.h"
#include "Weather.h"

#define SERIAL_BAUD_RATE 115200

#define WIFI_CONNECT_TIMEOUT_MS 30000UL
#define NTP_SYNC_TIMEOUT_MS 8000UL

constexpr const char* NTP_FALLBACK_SERVER = "pool.ntp.org";

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Config.mount();
  Config.load();

  WiFi.begin(Config.wifiSsid(), Config.wifiPassword());
  waitWifi(WIFI_CONNECT_TIMEOUT_MS);

  configTzTime(Config.timezone(), Config.ntpServer(), NTP_FALLBACK_SERVER);
  waitNtp(NTP_SYNC_TIMEOUT_MS);

  Weather.begin(Config.owmApiKey(), Config.owmLocation());
  Weather.request();
}

void loop() {
}
