#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Config {
 public:
  void mount();
  void load();
  void save();

  const char* otaPass() const;
  const char* owmApiKey() const;
  const char* owmLocation() const;
  const char* timezone() const;
  const char* ntpServer() const;

  void setOtaPass(const char* value);
  void setOwmApiKey(const char* value);
  void setOwmLocation(const char* value);
  void setTimezone(const char* value);
  void setNtpServer(const char* value);

 private:
  char otaPass_[16] = {0};
  char owmApiKey_[48] = {0};
  char owmLocation_[32] = {0};
  char timezone_[16] = {0};
  char ntpServer_[32] = {0};

  void applyDefaults();
  bool readFile(JsonDocument& doc);
  bool writeFile(const JsonDocument& doc);
  void convertFromJson(const JsonDocument& doc);
  void convertToJson(JsonDocument& doc) const;
};

extern Config AppConfig;

#endif  // CONFIG_H
