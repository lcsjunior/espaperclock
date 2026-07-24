#ifndef OPEN_WEATHER_MAP_H
#define OPEN_WEATHER_MAP_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

class OpenWeatherMap {
 public:
  void begin(const char* apiKey, const char* location);
  void refresh();

  float temperature() const;
  const char* description() const;

 private:
  WiFiClient wifiClient_;
  HTTPClient http_;
  const char* apiKey_ = nullptr;
  const char* location_ = nullptr;
  float temperature_ = 0.0f;
  char description_[48] = {0};
  unsigned long lastRefreshMs_ = 0;

  bool hasConfig() const;
  const char* formatUrl() const;
  void request();
  bool isStatusOk(int statusCode) const;
  bool parseJson(JsonDocument& doc);
  void processResponse(int statusCode);
  void setTemperature(float temp);
  void setDescription(const char* desc);
};

extern OpenWeatherMap Weather;

#endif  // OPEN_WEATHER_MAP_H
