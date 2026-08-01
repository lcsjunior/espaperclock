#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include <Arduino.h>

void waitWifi(uint32_t timeoutMs);
void waitNtp(uint32_t timeoutMs);
const char* formatDateTime();
bool isTimeSet();
void urlEncode(char* dest, size_t destSize, const char* src);

#endif  // CORE_UTILS_H
