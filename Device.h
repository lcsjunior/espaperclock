#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

void waitWifi(uint32_t timeoutMs);
void waitNtp(uint32_t timeoutMs);
const char* formatDateTime();
bool isTimeSet();

#endif  // DEVICE_H
