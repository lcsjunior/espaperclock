# espaperclock

ESP32 firmware (PlatformIO/Arduino) for a 1.54" e-paper clock. Shows a 24h
digital clock (NTP, configured timezone) and the current weather — condition
icon + temperature in °C — from the OpenWeatherMap *Current Weather* API.

> **Status:** the panel hasn't arrived — nothing is drawn yet, and `GxEPD2` /
> `Adafruit GFX` are out of `platformio.ini`.

## Display layout

| Normal | Fallback |
|:---:|:---:|
| <img src="assets/display-mockup.svg" alt="1.54 inch e-paper clock mockup: weather on top, date in the middle, large 24h time below" width="280"> | <img src="assets/display-mockup-fallback.svg" alt="Fallback mockup: dashed cloud with a question mark, and dashes in place of the temperature, date and time" width="280"> |

| Element | Content | Example | Fallback |
|---|---|---|---|
| Time | 24h `HH:MM`, the largest element | `14:37` | `--:--` |
| Temperature | current temp in °C, next to the weather icon | `18°C` | `--°C` |
| Date | weekday + `dd/mm` | `QUA 22/07` | `--- --/--` |

Each area degrades on its own: weather when OpenWeather fails, time and date
only on a never-synced board (no NTP, no RTC), rather than showing a plausible
wrong time. The mockup uses Bebas Neue; on-device that maps to a bitmap/GFX big
font.

## Hardware

Waveshare 1.54" e-Paper (V2) — 200×200 monochrome `SSD1681`, partial refresh —
on an ESP32, wired to the VSPI bus.

| E-paper | GPIO | Note |
|---|---|---|
| BUSY | GPIO4  | input |
| RST  | GPIO16 | output |
| DC   | GPIO17 | output |
| CS   | GPIO19 | VSPI MISO, free; avoids the strapping pin GPIO5 |
| CLK (SCK)  | GPIO18 | VSPI SCK |
| DIN (MOSI) | GPIO23 | VSPI MOSI |
| GND  | GND | |
| 3.3V | 3V3 | |

- [Lolin32 Lite pinout](assets/images/Lolin32_pinout03.png)
- [Waveshare 1.54" e-Paper — ESP32 wiring](https://www.waveshare.com/wiki/1.54inch_e-Paper_Module_Manual#ESP32.2F8266)

## HTTP endpoints

| Endpoint | Method | Auth | Description |
|---|---|---|---|
| `/health` | GET | none | Liveness check; returns status, datetime, temperature and weather. |

## Weather

OpenWeather *Current Weather Data*:

```
http://api.openweathermap.org/data/2.5/weather?q=Juiz%20de%20Fora,BR&units=metric&appid=<API_KEY>
```

Response fields used: `main.temp`, `weather[0].icon` / `weather[0].id` (icon
mapping), `weather[0].description`.

`OWM_HTTP_TIMEOUT_MS` and `OWM_REFRESH_INTERVAL_MS` are firmware tunables in
`platformio.ini`, not user settings.

## TODO

1. [x] Migrate all code to ESP32.
2. [ ] Move to the LOLIN C3 Pico (ESP32-C3): board, e-paper pinout, FSPI.
3. [ ] Deep sleep with NTP sync every 24h.
4. [ ] Display: partial refresh only; full refresh after NTP sync.
5. [ ] Run on the 600 mAh JST LiPo.
6. [ ] Retry the OpenWeather request on failure.
7. [ ] Cache the weather result (RTC memory) across deep sleep.
8. [ ] Move the OpenWeather URL into config.
9. [ ] Use HTTPS for the OpenWeather request.
