#include "Weather.h"

#include <ArduinoJson.h>
#include <LittleFS.h>
#include <new>

#include "Device.h"

#define OWM_HTTP_TIMEOUT_MS 8000UL

constexpr const char* OWM_URL =
    "https://api.openweathermap.org/data/2.5/"
    "weather?units=metric&q=%s&appid=%s";
constexpr const char* OWM_CA_CERT_PATH = "/owm-ca.pem";

WeatherClass Weather;

static char* caCert = nullptr;
RTC_DATA_ATTR static float cachedTemperature = 0.0f;
RTC_DATA_ATTR static char cachedDescription[48] = {0};
RTC_DATA_ATTR static char cachedCityName[48] = {0};

void WeatherClass::begin(const char* apiKey, const char* location) {
  apiKey_ = apiKey;
  location_ = location;
  loadCaCert();
}

void WeatherClass::request() {
  if (!hasConfig())
    return;

  wifiClient_.setCACert(caCert);
  http_.begin(wifiClient_, formatUrl());
  http_.setTimeout(OWM_HTTP_TIMEOUT_MS);
  const int statusCode = http_.GET();
  processResponse(statusCode);
  http_.end();
}

float WeatherClass::temperature() const {
  return cachedTemperature;
}

const char* WeatherClass::description() const {
  return cachedDescription;
}

const char* WeatherClass::cityName() const {
  return cachedCityName;
}

void WeatherClass::loadCaCert() {
  File file = LittleFS.open(OWM_CA_CERT_PATH, "r");
  if (!file) {
    log_e("Missing %s", OWM_CA_CERT_PATH);
    return;
  }

  const size_t size = file.size();
  caCert = new (std::nothrow) char[size + 1];
  if (!caCert) {
    log_e("Out of memory loading %s (%u bytes)", OWM_CA_CERT_PATH, size);
    file.close();
    return;
  }

  const size_t length = file.readBytes(caCert, size);
  file.close();
  caCert[length] = '\0';
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

  if (!caCert) {
    log_e("OWM CA cert not loaded");
    return false;
  }
  return true;
}

const char* WeatherClass::formatUrl() const {
  static char url[196];
  char encodedLocation[100];

  Device.urlEncode(encodedLocation, sizeof(encodedLocation), location_);
  snprintf(url, sizeof(url), OWM_URL, encodedLocation, apiKey_);

  return url;
}

bool WeatherClass::isStatusOk(int statusCode) const {
  if (statusCode != HTTP_CODE_OK) {
    log_e("OWM GET failed, code=%d", statusCode);
    return false;
  }
  return true;
}

bool WeatherClass::parseJson(JsonDocument& doc) {
  const DeserializationError err = deserializeJson(doc, http_.getStream());
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
  setCityName(doc["name"] | "");

  log_i("OWM %s %.2fC %s", cachedCityName, cachedTemperature,
        cachedDescription);
}

void WeatherClass::setTemperature(float value) {
  cachedTemperature = value;
}

void WeatherClass::setDescription(const char* value) {
  strlcpy(cachedDescription, value, sizeof(cachedDescription));
}

void WeatherClass::setCityName(const char* value) {
  strlcpy(cachedCityName, value, sizeof(cachedCityName));
}
