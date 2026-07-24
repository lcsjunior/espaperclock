#include "device.h"

#include <WiFi.h>
#include <clock.h>

#define LOADING_DOT_INTERVAL_MS 250UL

static uint32_t getChipId() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}

const char* getApName() {
  static char name[32];
  snprintf(name, sizeof(name), "ESPsoftAP-%lu", (unsigned long)getChipId());
  return name;
}

void urlEncode(char* dest, size_t destSize, const char* src) {
  static const char hexDigits[] = "0123456789ABCDEF";
  size_t j = 0;

  for (size_t i = 0; src[i] && j + 1 < destSize; ++i) {
    char c = src[i];
    if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' ||
        c == '~' || c == ',') {
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
