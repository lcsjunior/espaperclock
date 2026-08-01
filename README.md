# EPaperClock

ESP32 firmware (Arduino IDE / `arduino-cli`) for a 1.54" e-paper clock. Shows
a 24h digital clock (NTP, configured timezone).

> **Status:** the panel is wired and driven by a hello-world sketch — a
> full-screen partial refresh on every 1 min wake, over deep sleep, with no
> flashing. The clock layout below isn't drawn yet.

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
on a LOLIN C3 Pico (ESP32-C3), wired to the FSPI bus.

| E-paper | Function | LOLIN C3 Pico | Wire color |
|---|---|---|---|
| VCC  | 3.3V power        | 3V3 | ${\color{#808080}\textsf{Gray}}$ |
| GND  | Ground            | GND | ${\color{#8B4513}\textsf{Brown}}$ |
| DIN  | MOSI (SPI data)   | IO6 | ${\color{#1E6FD9}\textsf{Blue}}$ |
| CLK  | SCK (SPI clock)   | IO4 | ${\color{#D4A800}\textsf{Yellow}}$ |
| CS   | Chip Select       | IO3 | ${\color{#E8730C}\textsf{Orange}}$ |
| DC   | Data / Command    | IO2 | ${\color{#2E9E44}\textsf{Green}}$ |
| RST  | Reset             | IO1 | ${\color{#BDBDBD}\textsf{White}}$ |
| BUSY | Busy status       | IO0 | ${\color{#7B3FA0}\textsf{Purple}}$ |

- [LOLIN C3 Pico pinout](assets/images/wemos_c3_pico_pinout.png)
- [Waveshare 1.54" e-Paper — ESP32 wiring](https://www.waveshare.com/wiki/1.54inch_e-Paper_Module_Manual#ESP32.2F8266)

## Build, Upload

Board: **LOLIN C3 Pico** (`esp32:esp32:lolin_c3_pico`), pinned in
`sketch.yaml`.

```bash
arduino-cli compile --upload --profile lolin_c3_pico -p /dev/ttyACM0 .
arduino-cli monitor --profile lolin_c3_pico -p /dev/ttyACM0
./scripts/upload-littlefs.sh   # flashes data/config.json
```

`PORT`/`CHIP`/`BAUD`/`PARTITION_OFFSET`/`PARTITION_SIZE` are overridable env
vars on `upload-littlefs.sh`. No test suite — a clean compile/upload is the
definition of done.

## TODO

1. [x] Migrate all code to ESP32.
2. [x] Move to the LOLIN C3 Pico (ESP32-C3): board, e-paper pinout, FSPI.
3. [x] Deep sleep with NTP/Wheater sync every 1h.
4. [ ] Display: partial refresh only; full refresh every 24h.
5. [ ] Run on the 600 mAh JST LiPo.
6. [ ] Retry the OpenWeather request on failure.
7. [x] Cache the weather result (RTC memory) across deep sleep.
8. [x] Use HTTPS for the OpenWeather request.
9. [ ] Turn the display into a weather station only, dropping the clock;
   rename the project to InkWeather or PaperClima.
