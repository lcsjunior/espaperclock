#ifndef BUILTIN_LED_H
#define BUILTIN_LED_H

#include <Arduino.h>

class BuiltinLed {
 public:
  void begin(uint8_t pin);
  void blink();

 private:
  void on();
  void off();
};

extern BuiltinLed StatusLed;

#endif  // BUILTIN_LED_H
