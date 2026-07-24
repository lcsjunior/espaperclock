#include "BuiltinLed.h"

#include <Arduino.h>

#define BLINK_ON_MS 80UL
#define BLINK_OFF_MS 120UL

constexpr int BLINK_COUNT = 2;
constexpr int LEDC_CHANNEL = 0;
constexpr int LEDC_FREQ_HZ = 5000;
constexpr int LEDC_RESOLUTION_BITS = 8;
constexpr int DUTY_MAX = 255;
constexpr int BRIGHTNESS = 24;

BuiltinLed StatusLed;

void BuiltinLed::begin(uint8_t pin) {
  ledcSetup(LEDC_CHANNEL, LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
  ledcAttachPin(pin, LEDC_CHANNEL);

  off();
}

void BuiltinLed::blink() {
  for (int i = 0; i < BLINK_COUNT; ++i) {
    on();
    delay(BLINK_ON_MS);
    off();
    delay(BLINK_OFF_MS);
  }
}

void BuiltinLed::on() {
  ledcWrite(LEDC_CHANNEL, DUTY_MAX - BRIGHTNESS);
}

void BuiltinLed::off() {
  ledcWrite(LEDC_CHANNEL, DUTY_MAX);
}
