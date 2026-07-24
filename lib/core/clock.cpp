#include "clock.h"

#include <time.h>

constexpr int MIN_VALID_YEAR = 2024;

const char* formatDateTime() {
  static char dateTime[20];
  time_t now = time(nullptr);
  strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", localtime(&now));
  return dateTime;
}

bool isTimeSet() {
  const time_t now = time(nullptr);
  struct tm timeInfo;
  gmtime_r(&now, &timeInfo);
  return (timeInfo.tm_year + 1900) >= MIN_VALID_YEAR;
}
