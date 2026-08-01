#include "Weather.h"

#include <ArduinoJson.h>

#include "CoreUtils.h"

#define OWM_HTTP_TIMEOUT_MS 8000UL

constexpr const char* OWM_URL =
    "http://api.openweathermap.org/data/2.5/"
    "weather?units=metric&q=%s&appid=%s";

WeatherClass Weather;

void WeatherClass::begin(const char* apiKey, const char* location) {
  apiKey_ = apiKey;
  location_ = location;
}

float WeatherClass::temperature() const {
  return temperature_;
}

const char* WeatherClass::description() const {
  return description_;
}

bool WeatherClass::hasConfig() const {
  if (!apiKey_ || !apiKey_[0]) {
    log_w("OWM apiKey not configured");
    return false;
  }

  if (!location_ || !location_[0]) {
    log_w("OWM location not configured");
    return false;
  }
  return true;
}

const char* WeatherClass::formatUrl() const {
  static char url[196];
  char encodedLocation[100];

  urlEncode(encodedLocation, sizeof(encodedLocation), location_);
  snprintf(url, sizeof(url), OWM_URL, encodedLocation, apiKey_);

  return url;
}

void WeatherClass::request() {
  if (!hasConfig())
    return;

  http_.begin(wifiClient_, formatUrl());
  http_.setTimeout(OWM_HTTP_TIMEOUT_MS);
  const int statusCode = http_.GET();
  processResponse(statusCode);
  http_.end();
}

bool WeatherClass::isStatusOk(int statusCode) const {
  if (statusCode != HTTP_CODE_OK) {
    log_e("OWM GET failed, code=%d", statusCode);
    return false;
  }
  return true;
}

bool WeatherClass::parseJson(JsonDocument& doc) {
  DeserializationError err = deserializeJson(doc, http_.getStream());
  if (err) {
    log_e("OWM parse error: %s", err.c_str());
    return false;
  }
  return true;
}

void WeatherClass::processResponse(int statusCode) {
  if (!isStatusOk(statusCode))
    return;

  JsonDocument doc;
  if (!parseJson(doc))
    return;

  setTemperature(doc["main"]["temp"].as<float>());
  setDescription(doc["weather"][0]["description"] | "");

  log_i("OWM %.2fC %s", temperature_, description_);
}

void WeatherClass::setTemperature(float value) {
  temperature_ = value;
}

void WeatherClass::setDescription(const char* value) {
  strlcpy(description_, value, sizeof(description_));
}
