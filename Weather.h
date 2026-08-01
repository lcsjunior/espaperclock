#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

class WeatherClass {
 public:
  void begin(const char* apiKey, const char* location);
  void request();

  float temperature() const;
  const char* description() const;
  const char* cityName() const;

 private:
  WiFiClientSecure wifiClient_;
  HTTPClient http_;
  const char* apiKey_ = nullptr;
  const char* location_ = nullptr;

  void loadCaCert();
  bool hasConfig() const;
  const char* formatUrl() const;
  bool isStatusOk(int statusCode) const;
  bool parseJson(JsonDocument& doc);
  void processResponse(int statusCode);
  void setTemperature(float value);
  void setDescription(const char* value);
  void setCityName(const char* value);
};

extern WeatherClass Weather;

#endif  // WEATHER_H
