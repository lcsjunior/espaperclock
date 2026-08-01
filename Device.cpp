#include "Device.h"

#include <WiFi.h>
#include <cctype>
#include <time.h>

#define WIFI_CONNECT_TIMEOUT_MS 30000UL
#define NTP_SYNC_TIMEOUT_MS 8000UL
#define LOADING_DOT_INTERVAL_MS 250UL

constexpr int MIN_VALID_YEAR = 2024;
constexpr const char* NTP_FALLBACK_SERVER = "pool.ntp.org";

DeviceClass Device;

RTC_DATA_ATTR static uint32_t wakeCount = 0;

void DeviceClass::waitWifi() const {
  Serial.print("Waiting for WiFi connection...");

  const uint32_t startMs = millis();
  while (!WiFi.isConnected() &&
         (millis() - startMs) <= WIFI_CONNECT_TIMEOUT_MS) {
    Serial.print(".");
    delay(LOADING_DOT_INTERVAL_MS);
  }
  Serial.println(WiFi.isConnected() ? "connected" : "disconnected");
}

void DeviceClass::beginNtp(const char* timezone, const char* ntpServer) const {
  configTzTime(timezone, ntpServer, NTP_FALLBACK_SERVER);
}

void DeviceClass::waitNtp() const {
  Serial.print("Waiting for NTP sync...");

  const uint32_t startMs = millis();
  while (!isTimeSet() && (millis() - startMs) <= NTP_SYNC_TIMEOUT_MS) {
    Serial.print(".");
    delay(LOADING_DOT_INTERVAL_MS);
  }
  Serial.printf(isTimeSet() ? "synced (%s)\n" : "not synced\n",
                formatDateTime());
}

void DeviceClass::setTimezone(const char* timezone) const {
  setenv("TZ", timezone, 1);
  tzset();
}

const char* DeviceClass::formatDateTime() const {
  static char dateTime[20];
  const time_t now = time(nullptr);
  strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return dateTime;
}

bool DeviceClass::isTimeSet() const {
  const time_t now = time(nullptr);
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);
  return (timeInfo.tm_year + 1900) >= MIN_VALID_YEAR;
}

void DeviceClass::urlEncode(char* dest, size_t destSize,
                            const char* src) const {
  constexpr char hexDigits[] = "0123456789ABCDEF";
  size_t j = 0;

  for (size_t i = 0; src[i] && j + 1 < destSize; ++i) {
    const char c = src[i];
    if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' ||
        c == '.' || c == '~' || c == ',') {
      dest[j++] = c;
      continue;
    }
    if (j + 3 >= destSize)
      break;
    dest[j++] = '%';
    dest[j++] = hexDigits[(c >> 4) & 0x0F];
    dest[j++] = hexDigits[c & 0x0F];
  }
  dest[j] = '\0';
}

bool DeviceClass::syncDue() const {
  return wakeCount == 0;
}

void DeviceClass::deepSleep(uint32_t intervalS,
                            uint32_t syncIntervalWakes) const {
  wakeCount = (wakeCount + 1) % syncIntervalWakes;
  log_i("Wake %lu/%lu, entering deep sleep for %lu s", wakeCount,
        syncIntervalWakes, intervalS);
  ESP.deepSleep(intervalS * 1000000ULL);
}
