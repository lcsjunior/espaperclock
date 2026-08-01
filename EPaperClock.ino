#include <Arduino.h>

#include <WiFi.h>

#include "Config.h"
#include "Sys.h"
#include "Weather.h"

#define SERIAL_BAUD_RATE 115200
#define DEEP_SLEEP_INTERVAL_S 60UL
#define SYNC_INTERVAL_WAKES 60UL

static void resync() {
  WiFi.begin(Config.wifiSsid(), Config.wifiPassword());
  Sys.waitWifi();

  Sys.beginNtp(Config.timezone(), Config.ntpServer());
  Sys.waitNtp();

  Weather.begin(Config.owmApiKey(), Config.owmLocation());
  Weather.request();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Config.mount();
  Config.load();

  Sys.everyCycle(resync);

  Sys.setTimezone(Config.timezone());

  Sys.deepSleep(DEEP_SLEEP_INTERVAL_S, SYNC_INTERVAL_WAKES);
}

void loop() {
}
