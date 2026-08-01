#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

class ConfigClass {
 public:
  void mount();
  void load();

  const char* wifiSsid() const;
  const char* wifiPassword() const;
  const char* timezone() const;
  const char* ntpServer() const;

 private:
  char wifiSsid_[33] = {0};
  char wifiPassword_[64] = {0};
  char timezone_[16] = {0};
  char ntpServer_[32] = {0};
};

extern ConfigClass Config;

#endif  // CONFIG_H
