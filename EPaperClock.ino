#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h>
#include <SPI.h>
#include <WiFi.h>

#include "Config.h"
#include "Sys.h"
#include "Weather.h"

#define SERIAL_BAUD_RATE 115200
#define DEEP_SLEEP_INTERVAL_S 60UL
#define SYNC_INTERVAL_WAKES 60UL

#define EPD_SCK 4
#define EPD_MOSI 6
#define EPD_CS 3
#define EPD_DC 2
#define EPD_RST 1
#define EPD_BUSY 0
#define EPD_RESET_DURATION_MS 50

constexpr uint16_t textX = 10;
constexpr uint16_t greetingY = 30;
constexpr uint16_t subtitleY = 70;
constexpr uint16_t counterY = 110;

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
    GxEPD2_154_D67(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

constexpr uint8_t bufferBlack = 0x00;
constexpr uint8_t bufferWhite = 0xFF;

RTC_DATA_ATTR static int counter = 0;

static void drawScreen() {
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setTextSize(2);
    display.setCursor(textX, greetingY);
    display.println("Hello World!");
    display.setTextSize(1);
    display.setCursor(textX, subtitleY);
    display.println("Partial Mode V2");
    display.setTextSize(2);
    display.setCursor(textX, counterY);
    display.print("Count: ");
    display.print(counter);
  } while (display.nextPage());
}

static void refreshDisplay() {
  SPI.begin(EPD_SCK, -1, EPD_MOSI, EPD_CS);

  const bool coldBoot = counter == 0;

  display.init(SERIAL_BAUD_RATE, coldBoot, EPD_RESET_DURATION_MS, false);
  display.setRotation(1);

  counter++;

  if (coldBoot) {
    drawScreen();
  } else {
    display.epd2.writeScreenBufferAgain(bufferBlack);
    drawScreen();

    display.epd2.writeScreenBufferAgain(bufferWhite);
    drawScreen();
  }

  display.hibernate();
}

static void resync() {
  WiFi.begin(Config.wifiSsid(), Config.wifiPassword());
  Sys.waitWifi();

  Sys.beginNtp(Config.timezone(), Config.ntpServer());
  Sys.waitNtp();

  Weather.begin(Config.owmApiKey(), Config.owmLocation());
  Weather.request();
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);

  Config.mount();
  Config.load();

  Sys.setTimezone(Config.timezone());
  Sys.everyCycle(resync);

  refreshDisplay();

  Sys.deepSleep(DEEP_SLEEP_INTERVAL_S, SYNC_INTERVAL_WAKES);
}

void loop() {
}
