#include <Arduino.h>

#include <WiFi.h>

#include "Config.h"
#include "Device.h"
#include "Weather.h"

#define SERIAL_BAUD_RATE 115200
#define DEEP_SLEEP_INTERVAL_S 60UL
#define SYNC_INTERVAL_WAKES 60UL

static void resync() {
  if (!Device.shouldSync())
    return;

  WiFi.begin(Config.wifiSsid(), Config.wifiPassword());
  Device.waitWifi();

  Device.beginNtp(Config.timezone(), Config.ntpServer());
  Device.waitNtp();

  Weather.begin(Config.owmApiKey(), Config.owmLocation());
  Weather.request();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Config.mount();
  Config.load();

  resync();

  Device.setTimezone(Config.timezone());

  Device.deepSleep(DEEP_SLEEP_INTERVAL_S, SYNC_INTERVAL_WAKES);
}

void loop() {
}
