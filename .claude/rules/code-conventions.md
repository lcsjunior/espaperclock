# Project Conventions

Target: **ESP32** only. Hardware constraints (pinout, timing, memory) override
any rule here.

## Memory

- No `malloc()`/`new` at runtime — allocate only in global constructors.
- No Arduino `String` — use `char[]` + `snprintf`.
- Fixed-size buffers, sized from the protocol/payload.
- `static` a buffer only if it's returned by pointer or it's a `const` lookup
  table; otherwise keep it local. The 8 KB loop stack has room, and `static`
  costs RAM forever and breaks reentrancy.
- Diagnose with `ESP.getFreeHeap()` / `ESP.getMinFreeHeap()`.

## Flash strings

Forbidden: `PROGMEM`, `PSTR`, `FPSTR`, `F()`, `_P` variants — flash is
memory-mapped, so they're no-ops. Use plain literals or `constexpr const
char*`, copied with `strlcpy`.

## Logging

- Use `log_e`/`log_w`/`log_i`/`log_d` — plain literals, no `F()`/`PSTR`, no
  trailing `\n`.
- `Tools > Core Debug Level` in the Arduino IDE (`Info` or above) is what
  makes `log_i` emit; for `arduino-cli` it's the FQBN's `DebugLevel` option.
- `Serial.print*` only in the wait helpers (`Device.cpp`), for progress dots.

## Constants

- Default to `constexpr`.
- `#define` (`UPPER_SNAKE_CASE`) only for pins, timeouts/intervals, and fixed
  peripheral settings (e.g. `SERIAL_BAUD_RATE`), with a unit suffix — ours are
  `_MS`/`UL`, a library call keeps that library's own unit.
- `SERIAL_BAUD_RATE` is always `115200`.
- No `#ifndef` fallback guards — the Arduino IDE build has no `build_flags` to
  feed them.

## Timing

- No `delay()` in `loop()` — use `millis()` elapsed-time checks.
- `loop()` shares the CPU with WiFi via FreeRTOS — keep iterations short.

## ISR / callbacks

- Only set flags or toggle outputs — no `delay()`, `Serial`, sensor reads.
- Mark `IRAM_ATTR`.
- `Wire`/`SPI`/`Serial` only from the main loop.

## Hardware access

Go through the abstraction layer, never the driver directly (throttling lives
there). A direct call needs a comment explaining why.

## Includes

`.cpp`: own header, blank line, `<angle>` includes (alphabetical), blank line,
local `"quoted"` includes. Headers include only what their declarations need;
the rest goes in the `.cpp`.

## Naming

Arduino convention overrides skill defaults:

- `PascalCase`: classes/types (`Weather`, `AppConfig`).
- `camelCase`: everything else — methods, functions, variables, attributes.
- `UPPER_SNAKE_CASE`: macros, pins.
- Buffers named for content (`url`, `payload`), never `buf`/`Buf`.
- Interfaces take the plain name; on collision with a concrete class, the
  class gets the `Class` suffix (`WiFiClass`/`WiFi`).
- Singletons are `PascalCase` (`Serial`, `Wire`) — `extern` in the header,
  defined at the top of the `.cpp`.
- A file takes its class's name (`Config.h`); if the class has the `Class`
  suffix, the file takes the singleton's name instead (`Led.h` for
  `LedClass`).
- Every sketch file is `PascalCase`, class or not (`Device.h`), like
  `Arduino.h` itself.

## Style

- English only — code, identifiers, logs.
- No comments — names carry intent.
- Class layout: `public` (ctor first) → `protected` → `private` (attributes
  then methods); `.cpp` follows header order.
- Blank line separates guard clauses, body, and result — skip when there's
  nothing to separate.
- `const` on non-mutating methods and never-reassigned locals.
- Prefer early return and the ternary over nested `if/else`.
- Unused return value → make the function `void` and log failure internally.

## Dependencies

Pinned in `sketch.yaml` (`esp32:esp32` core version, library versions),
matching what's installed via the Arduino IDE's Boards/Library Manager. No
add/remove/update without explicit approval.

## Sensitive data

- No secrets (credentials, keys, tokens) in code, logs, commits, docs, or
  output — they live in `data/config.json` (git-ignored).
- No usernames in paths — `~` instead of `/home/<user>`.

## Documentation

Keep `README.md` (product) and `CLAUDE.md` (repo workflow) current together,
without overlap. `README.md`'s TODO list is the user's — the only allowed
edit is `[ ]` → `[x]`.
