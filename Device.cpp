#include "Device.h"

#include <WiFi.h>
#include <time.h>

#define LOADING_DOT_INTERVAL_MS 250UL

constexpr int MIN_VALID_YEAR = 2024;

void waitWifi(uint32_t timeoutMs) {
  Serial.print("Waiting for WiFi connection...");

  const uint32_t startMs = millis();
  while (!WiFi.isConnected() && (millis() - startMs) <= timeoutMs) {
    Serial.print(".");
    delay(LOADING_DOT_INTERVAL_MS);
  }
  Serial.println(WiFi.isConnected() ? "connected" : "disconnected");
}

void waitNtp(uint32_t timeoutMs) {
  Serial.print("Waiting for NTP sync...");

  const uint32_t startMs = millis();
  while (!isTimeSet() && (millis() - startMs) <= timeoutMs) {
    Serial.print(".");
    delay(LOADING_DOT_INTERVAL_MS);
  }
  Serial.printf(isTimeSet() ? "synced (%s)\n" : "not synced\n",
                formatDateTime());
}

const char* formatDateTime() {
  static char dateTime[20];
  time_t now = time(nullptr);
  strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return dateTime;
}

bool isTimeSet() {
  const time_t now = time(nullptr);
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);
  return (timeInfo.tm_year + 1900) >= MIN_VALID_YEAR;
}
