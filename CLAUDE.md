# CLAUDE.md

Guidance for Claude Code (claude.ai/code) when working in this repository.

## Overview

ESP32 firmware for a 1.54" e-paper clock — see `README.md`. The panel hasn't
arrived: WiFi provisioning, settings, OTA, NTP time sync and OpenWeatherMap
fetch exist; display code does not.

**Migration in progress**: the project is moving from PlatformIO to the
Arduino IDE. The pre-migration source (matching the layout below) currently
sits under `old/` for reference; `EPaperClock.ino` is the new sketch, not yet
populated. Until the migration lands, treat the paths below as describing
`old/<path>`, and validate the "Build, Upload" section against whichever
toolchain is actually in use before relying on it.

## Architecture (where things live)

- `src/main.cpp` — boot sequence and `loop()`.
- `src/modules/` — `wifi_setup`: `initWifi()` does the captive portal, custom
  fields, save callback, `configTzTime` and OTA; `http_server` (routes go on the
  WiFiManager portal server, after `initWifi()`).
- `lib/fs/` — `AppConfig`: LittleFS + ArduinoJson persistence of `/config.json`.
- `lib/weather/` — `Weather` (`OpenWeatherMap`): *Current Weather* fetch,
  throttled internally; `refresh()` runs every `loop()`, reads are cached.
- `lib/core/` — `clock`: time formatting and NTP-synced check; `device`: URL
  encode, AP name, WiFi/NTP wait.

Anything user-configurable belongs in the portal plus `Config`, not in build
flags.

## Build, Upload

```bash
pio run                  # compile
pio run --target upload  # flash over USB
pio device monitor       # serial monitor @ 115200 baud
```

Single env; the board is named only in `platformio.ini`. There is no test
suite — a clean `pio run` is the definition of done.

- **Always ask for explicit user confirmation before any upload/flash** — it
  writes to physical hardware.
- USB is the default upload path; keep it that way. OTA is opt-in per run or via
  an untracked `local_settings.ini`.

## Conventions

**All code must follow `.claude/rules/code-conventions.md`** — the single source
of truth. Read it before writing or reviewing.
