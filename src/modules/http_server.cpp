#include "http_server.h"

#include <clock.h>
#include <OpenWeatherMap.h>

#include "wifi_setup.h"

constexpr const char* APPLICATION_JSON = "application/json";

void initHttpServer() {
  wifiManager.server->on("/health", HTTP_GET, []() {
    char payload[192];
    snprintf(payload, sizeof(payload),
             "{\"status\":\"UP\",\"dateTime\":\"%s\",\"temperature\":%.2f,"
             "\"weather\":\"%s\"}",
             formatDateTime(), Weather.temperature(), Weather.description());
    wifiManager.server->send(200, APPLICATION_JSON, payload);
  });
}
