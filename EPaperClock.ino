#include <Arduino.h>

#include <WiFi.h>

#include "Config.h"
#include "CoreUtils.h"
#include "Weather.h"

#define SERIAL_BAUD_RATE 115200

#define WIFI_CONNECT_TIMEOUT_MS 30000UL
#define NTP_SYNC_TIMEOUT_MS 8000UL
#define DEEP_SLEEP_INTERVAL_S 60UL

constexpr const char* NTP_FALLBACK_SERVER = "pool.ntp.org";
constexpr uint32_t SYNC_INTERVAL_WAKES = 60;

RTC_DATA_ATTR static uint32_t wakeCount = 0;

static bool shouldSync() {
  return wakeCount == 0;
}

static void resync() {
  if (!shouldSync())
    return;

  WiFi.begin(Config.wifiSsid(), Config.wifiPassword());
  waitWifi(WIFI_CONNECT_TIMEOUT_MS);

  configTzTime(Config.timezone(), Config.ntpServer(), NTP_FALLBACK_SERVER);
  waitNtp(NTP_SYNC_TIMEOUT_MS);

  Weather.begin(Config.owmApiKey(), Config.owmLocation());
  Weather.request();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Config.mount();
  Config.load();

  setTimezone(Config.timezone());

  resync();

  wakeCount = (wakeCount + 1) % SYNC_INTERVAL_WAKES;
  log_i("Wake %lu/%lu, entering deep sleep for %lu s", wakeCount,
        SYNC_INTERVAL_WAKES, DEEP_SLEEP_INTERVAL_S);
  ESP.deepSleep(DEEP_SLEEP_INTERVAL_S * 1000000ULL);
}

void loop() {
}
