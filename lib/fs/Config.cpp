#include "Config.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

constexpr const char* CONFIG_PATH = "/config.json";

constexpr const char* DEFAULT_OWM_LOCATION = "Juiz de Fora,BR";
constexpr const char* DEFAULT_TIMEZONE = "BRT3";
constexpr const char* DEFAULT_NTP_SERVER = "br.pool.ntp.org";

Config AppConfig;

void Config::mount() {
  if (LittleFS.begin()) {
    log_i("LittleFS mounted");
    return;
  }

  log_w("Failed to mount LittleFS, formatting");

  if (!LittleFS.format() || !LittleFS.begin()) {
    log_e("Failed to format LittleFS");
    return;
  }
  log_i("LittleFS formatted and mounted");
}

void Config::load() {
  applyDefaults();

  JsonDocument doc;
  if (!readFile(doc))
    return;

  convertFromJson(doc);
}

void Config::save() {
  JsonDocument doc;
  convertToJson(doc);
  writeFile(doc);
}

const char* Config::otaPass() const {
  return otaPass_;
}

const char* Config::owmApiKey() const {
  return owmApiKey_;
}

const char* Config::owmLocation() const {
  return owmLocation_;
}

const char* Config::timezone() const {
  return timezone_;
}

const char* Config::ntpServer() const {
  return ntpServer_;
}

void Config::setOtaPass(const char* value) {
  strlcpy(otaPass_, value, sizeof(otaPass_));
}

void Config::setOwmApiKey(const char* value) {
  strlcpy(owmApiKey_, value, sizeof(owmApiKey_));
}

void Config::setOwmLocation(const char* value) {
  strlcpy(owmLocation_, value, sizeof(owmLocation_));
}

void Config::setTimezone(const char* value) {
  strlcpy(timezone_, value, sizeof(timezone_));
}

void Config::setNtpServer(const char* value) {
  strlcpy(ntpServer_, value, sizeof(ntpServer_));
}

void Config::applyDefaults() {
  setOtaPass(OTA_PASS);
  setOwmApiKey("");
  setOwmLocation(DEFAULT_OWM_LOCATION);
  setTimezone(DEFAULT_TIMEZONE);
  setNtpServer(DEFAULT_NTP_SERVER);
}

bool Config::readFile(JsonDocument& doc) {
  File file = LittleFS.open(CONFIG_PATH, "r");
  if (!file) {
    log_w("No %s yet, keeping defaults", CONFIG_PATH);
    return false;
  }

  DeserializationError err = deserializeJson(doc, file);
  const size_t bytesRead = file.size();
  file.close();

  if (err) {
    log_e("Parse error: %s", err.c_str());
    return false;
  }

  log_i("Loaded %u bytes from %s", bytesRead, CONFIG_PATH);
  return true;
}

bool Config::writeFile(const JsonDocument& doc) {
  File file = LittleFS.open(CONFIG_PATH, "w");
  if (!file) {
    log_e("Failed to open %s for writing", CONFIG_PATH);
    return false;
  }

  const size_t bytesWritten = serializeJson(doc, file);
  file.close();

  if (bytesWritten == 0) {
    log_e("Failed to write to %s", CONFIG_PATH);
    return false;
  }

  log_i("Saved %u bytes to %s", bytesWritten, CONFIG_PATH);
  return true;
}

void Config::convertFromJson(const JsonDocument& doc) {
  setOtaPass(doc["ota_pass"] | otaPass_);
  setOwmApiKey(doc["owm_api_key"] | owmApiKey_);
  setOwmLocation(doc["owm_location"] | owmLocation_);
  setTimezone(doc["timezone"] | timezone_);
  setNtpServer(doc["ntp_server"] | ntpServer_);
}

void Config::convertToJson(JsonDocument& doc) const {
  doc["ota_pass"] = otaPass_;
  doc["owm_api_key"] = owmApiKey_;
  doc["owm_location"] = owmLocation_;
  doc["timezone"] = timezone_;
  doc["ntp_server"] = ntpServer_;
}
