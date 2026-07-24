#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

const char* getApName();
void urlEncode(char* dest, size_t destSize, const char* src);
void waitWifi(uint32_t timeoutMs);
void waitNtp(uint32_t timeoutMs);

#endif  // DEVICE_H
