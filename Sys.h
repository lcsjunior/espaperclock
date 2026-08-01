#ifndef SYS_H
#define SYS_H

#include <Arduino.h>

class SysClass {
 public:
  void waitWifi() const;
  void beginNtp(const char* timezone, const char* ntpServer) const;
  void waitNtp() const;
  void setTimezone(const char* timezone) const;
  const char* formatDateTime() const;
  bool isTimeSet() const;
  void everyCycle(void (*action)()) const;
  void deepSleep(uint32_t intervalS, uint32_t syncIntervalWakes) const;
};

extern SysClass Sys;

void urlEncode(char* dest, size_t destSize, const char* src);

#endif  // SYS_H
