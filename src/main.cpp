#include <Arduino.h>
#include <BuiltinLed.h>
#include <Config.h>
#include <OpenWeatherMap.h>

#include "modules/http_server.h"
#include "modules/wifi_setup.h"

#define SERIAL_BAUD_RATE 115200
#define STATUS_LED_PIN 22

void setup() {
  StatusLed.begin(STATUS_LED_PIN);
  StatusLed.blink();

  Serial.begin(SERIAL_BAUD_RATE);

  AppConfig.mount();
  AppConfig.load();

  initWifi();
  Weather.begin(AppConfig.owmApiKey(), AppConfig.owmLocation());
  initHttpServer();
}

void loop() {
  handleWifi();
  Weather.refresh();
}
