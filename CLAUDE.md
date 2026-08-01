# CLAUDE.md

Guidance for Claude Code (claude.ai/code) when working in this repository.

## Overview

ESP32-C3 firmware for a 1.54" e-paper clock — see `README.md`. The panel
hasn't arrived, so there's no display code yet.

Implemented: WiFi connect, NTP time sync, OpenWeatherMap fetch, deep sleep
with periodic resync. Planned: display (see `README.md`'s TODO).

Toolchain: Arduino IDE plus `arduino-cli` for reproducible/scripted builds.

## Architecture

- `EPaperClock.ino` — boot sequence, `setup()`/`loop()`. Deep sleeps 1 min at
  a time (future display redraw cadence) via `Device.deepSleep()`; WiFi/NTP/
  weather resync only runs every 60th wake (~1h), gated by
  `Device.shouldSync()`. `Config.mount()`/`load()` run every wake regardless
  (cheap, no network), so `TZ` can be reapplied (`setenv`/`tzset`) from it on
  every wake — deep sleep clears the libc environment, so skipping this on
  non-sync wakes would make `localtime()` read UTC until the next resync.
- `Config.h`/`Config.cpp` — `ConfigClass` (singleton `Config`): read-only
  LittleFS + ArduinoJson load of `/config.json` — WiFi SSID/password,
  timezone, NTP server.
- `Device.h`/`Device.cpp` — `DeviceClass` (singleton `Device`): `waitWifi()`/
  `beginNtp()`/`waitNtp()` wait/sync helpers, `formatDateTime()`/`isTimeSet()`/
  `urlEncode()`, and the deep sleep/wake-counter cycle (`shouldSync()`,
  `deepSleep()`) — the wake counter is `RTC_DATA_ATTR`, so it survives deep
  sleep. Never takes a `Config` dependency; callers pass in whatever
  device-agnostic values it needs (e.g. timezone, NTP server).
- `Weather.h`/`Weather.cpp` — `WeatherClass` (singleton `Weather`): HTTPS
  OpenWeatherMap fetch, TLS-pinned via `data/owm-ca.pem`. Temperature and
  description are cached in RTC memory (`RTC_DATA_ATTR`), so the last known
  reading survives deep sleep and a failed fetch just leaves it in place.
- `data/config.json` — the actual secrets, git-ignored, flashed to the
  device separately from the sketch (see Build, Upload). Never written by
  the firmware. `data/config.json.example` is the tracked template.
- `data/owm-ca.pem` — root CA pinned for the OpenWeatherMap HTTPS request
  (`Weather.cpp`), git-ignored, flashed the same way as `config.json`.
  `data/owm-ca.pem.example` documents how to fetch it.
- `sketch.yaml` — pins the `esp32:esp32` core version and library versions
  for `arduino-cli`; not read by the Arduino IDE GUI.

Every source file lives flat at the repo root, next to `EPaperClock.ino`, so
it shows up as a tab in the Arduino IDE — see `.claude/rules/code-conventions.md`.

User-configurable values belong in `Config`, sourced from `data/config.json`
— never hardcoded, never in build flags.

## Build, Upload

Board: **LOLIN C3 Pico** (`esp32:esp32:lolin_c3_pico`), pinned in
`sketch.yaml`. Partition Scheme is **Huge APP (3MB No OTA/1MB SPIFFS)** —
no OTA, all uploads are over serial. `Tools > Partition Scheme` in the IDE
GUI must match (`sketch.yaml` doesn't apply there, same caveat as `DebugLevel`
in `code-conventions.md`); a mismatch flashes LittleFS data at the wrong
offset and corrupts the app partition.

```bash
arduino-cli compile --upload --profile lolin_c3_pico -p /dev/ttyACM0 .
arduino-cli monitor --profile lolin_c3_pico -p /dev/ttyACM0
./scripts/upload-littlefs.sh   # flashes data/config.json
```

`PORT`/`CHIP`/`BAUD`/`PARTITION_OFFSET`/`PARTITION_SIZE` are overridable env
vars on `upload-littlefs.sh`. No test suite — a clean compile/upload is the
definition of done.

- **Always ask for explicit user confirmation before any upload/flash** — it
  writes to physical hardware.

## Conventions

**All code must follow `.claude/rules/code-conventions.md`** — the single
source of truth. Read it before writing or reviewing.
