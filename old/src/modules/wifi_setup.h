#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFiManager.h>

extern WiFiManager wifiManager;

void initWifi();
void handleWifi();

#endif  // WIFI_SETUP_H
