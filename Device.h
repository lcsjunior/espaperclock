#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

class DeviceClass {
 public:
  void waitWifi() const;
  void beginNtp(const char* timezone, const char* ntpServer) const;
  void waitNtp() const;
  void setTimezone(const char* timezone) const;
  const char* formatDateTime() const;
  bool isTimeSet() const;
  void urlEncode(char* dest, size_t destSize, const char* src) const;
  bool syncDue() const;
  void deepSleep(uint32_t intervalS, uint32_t syncIntervalWakes) const;
};

extern DeviceClass Device;

#endif  // DEVICE_H
