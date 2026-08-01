#include "CoreUtils.h"

#include <WiFi.h>
#include <cctype>
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

void setTimezone(const char* timezone) {
  setenv("TZ", timezone, 1);
  tzset();
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

void urlEncode(char* dest, size_t destSize, const char* src) {
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
