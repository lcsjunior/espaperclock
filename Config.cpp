#include "Config.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

constexpr const char* CONFIG_PATH = "/config.json";

constexpr const char* DEFAULT_TIMEZONE = "BRT3";
constexpr const char* DEFAULT_NTP_SERVER = "br.pool.ntp.org";

ConfigClass Config;

void ConfigClass::mount() {
  if (LittleFS.begin()) {
    log_i("LittleFS mounted");
    return;
  }
  log_e("Failed to mount LittleFS");
}

void ConfigClass::load() {
  File file = LittleFS.open(CONFIG_PATH, "r");
  if (!file) {
    log_e("Missing %s", CONFIG_PATH);
    return;
  }

  JsonDocument doc;
  const DeserializationError err = deserializeJson(doc, file);
  file.close();

  if (err) {
    log_e("Parse error: %s", err.c_str());
    return;
  }

  strlcpy(wifiSsid_, doc["wifi_ssid"] | "", sizeof(wifiSsid_));
  strlcpy(wifiPassword_, doc["wifi_password"] | "", sizeof(wifiPassword_));
  strlcpy(timezone_, doc["timezone"] | DEFAULT_TIMEZONE, sizeof(timezone_));
  strlcpy(ntpServer_, doc["ntp_server"] | DEFAULT_NTP_SERVER,
          sizeof(ntpServer_));
  strlcpy(owmApiKey_, doc["owm_api_key"] | "", sizeof(owmApiKey_));
  strlcpy(owmLocation_, doc["owm_location"] | "", sizeof(owmLocation_));
}

const char* ConfigClass::wifiSsid() const {
  return wifiSsid_;
}

const char* ConfigClass::wifiPassword() const {
  return wifiPassword_;
}

const char* ConfigClass::timezone() const {
  return timezone_;
}

const char* ConfigClass::ntpServer() const {
  return ntpServer_;
}

const char* ConfigClass::owmApiKey() const {
  return owmApiKey_;
}

const char* ConfigClass::owmLocation() const {
  return owmLocation_;
}
