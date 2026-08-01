# CLAUDE.md

Guidance for Claude Code (claude.ai/code) when working in this repository.

## Overview

ESP32-C3 firmware for a 1.54" e-paper clock — see `README.md`. The panel
hasn't arrived, so there's no display code yet.

Implemented: WiFi connect, NTP time sync. Planned: display, deep sleep,
OpenWeatherMap fetch (see `README.md`'s TODO).

Toolchain: Arduino IDE plus `arduino-cli` for reproducible/scripted builds.
`old/` is reference-only — never edit it, and don't use it to infer what's
pending.

## Architecture

- `EPaperClock.ino` — boot sequence, `setup()`/`loop()`.
- `Config.h`/`Config.cpp` — `ConfigClass` (singleton `Config`): read-only
  LittleFS + ArduinoJson load of `/config.json` — WiFi SSID/password,
  timezone, NTP server.
- `Device.h`/`Device.cpp` — `waitWifi()`/`waitNtp()` wait helpers,
  `formatDateTime()`/`isTimeSet()`.
- `data/config.json` — the actual secrets, git-ignored, flashed to the
  device separately from the sketch (see Build, Upload). Never written by
  the firmware. `data/config.json.example` is the tracked template.
- `sketch.yaml` — pins the `esp32:esp32` core version and library versions
  for `arduino-cli`; not read by the Arduino IDE GUI.

Every source file lives flat at the repo root, next to `EPaperClock.ino`, so
it shows up as a tab in the Arduino IDE — see `.claude/rules/code-conventions.md`.

User-configurable values belong in `Config`, sourced from `data/config.json`
— never hardcoded, never in build flags.

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

- **Always ask for explicit user confirmation before any upload/flash** — it
  writes to physical hardware.

## Conventions

**All code must follow `.claude/rules/code-conventions.md`** — the single
source of truth. Read it before writing or reviewing.
