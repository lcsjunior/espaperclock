# Project Conventions

Target is the **ESP32** only.

When a rule here conflicts with a hardware constraint (pinout, timing, memory),
the hardware wins.

## Memory

- No `malloc()`/`new` at runtime; allocate in global constructors only.
- Never use Arduino `String` — `char[]` + `snprintf`.
- Fixed-size buffers, sized from the protocol/payload.
- `static` a buffer only when its pointer outlives the call (the function
  returns it) or when it is a `const` lookup table. The loop task has an 8 KB
  stack, so a few hundred bytes of local buffer are fine — an unnecessary
  `static` costs RAM forever and makes the function non-reentrant.
- Diagnose with `ESP.getFreeHeap()` and `ESP.getMinFreeHeap()`.

## Flash strings

**Forbidden** — flash is memory-mapped, so the machinery is a no-op: no
`PROGMEM`, `PSTR`, `FPSTR`, `F()`, no `_P` variant (`snprintf_P`, `strcpy_P`, …).
Constant strings are plain literals or `constexpr const char*`, copied with
`strlcpy`. No exceptions.

## Logging

- Use the core macros: `log_e`, `log_w`, `log_i`, `log_d`.
- Plain literal formats, no `F()`/`PSTR`, no trailing `\n` — the macro adds it.
- `-DCORE_DEBUG_LEVEL=3` in `platformio.ini` is what makes `log_i` emit.
- `Serial.print*` is reserved for the wait helpers in `lib/core/device.cpp`,
  which print progress dots inline.

## Constants

- Default to `constexpr` (`constexpr const char*`, `constexpr int`, …).
- `#define` (`UPPER_SNAKE_CASE`) only for pins and for timeouts/intervals, with
  a suffix stating the unit. Our own throttles and waits are `_MS` with a `UL`
  literal; a value handed to a library adopts that library's unit (`_S` for
  `setConfigPortalTimeout`), never converted at the call site.
- A macro fed by a build flag *may* carry an `#ifndef` fallback at the top of
  each `.cpp` consuming it — optional, for tunables with a sensible default.

## Timing

- Never `delay()` in `loop()`; use elapsed-time checks against `millis()`.
- `loop()` is a FreeRTOS task sharing the CPU with the WiFi stack — keep
  iterations short so the task watchdog stays fed.

## ISR / callbacks

- Handlers set flags or toggle outputs, nothing else. No `delay()`, `Serial`,
  or sensor reads.
- Mark them `IRAM_ATTR`.
- `Wire`, `SPI` and `Serial` only from the main loop.

## Hardware access

- Reach peripherals through their abstraction layer, never the driver directly;
  assume throttling lives inside it. A direct call needs a comment saying why.

## Includes

- A `.cpp` opens with its own header, then a blank line, then `<angle>` includes
  in alphabetical order, then a blank line and any local `"quoted"` headers.
- A header includes only what its own declarations need. Anything the
  implementation alone uses belongs in the `.cpp`.

## Naming

Arduino convention, which overrides any naming recommended in skills:

- `PascalCase` — classes and types.
- `camelCase` — methods, functions, variables, attributes.
- `UPPER_SNAKE_CASE` — macros and pins.
- Buffers are named for what they hold (`url`, `payload`, `dateTime`), never
  `buf` nor a `Buf` suffix.
- Interfaces take the plain name (`TemperatureSensor`, no `I` prefix); when it
  collides with a concrete class, that class gets the `Class` suffix, like the
  Arduino core (`extern WiFiClass WiFi;`).
- Global singletons are `PascalCase` like the Arduino core libs (`Serial`,
  `Wire`): `extern DisplayManager Display;` in the header, defined at the top of
  the `.cpp`, right after the includes.
- A file that holds a class takes that class's name verbatim (`Config.h`,
  `OpenWeatherMap.cpp`); when the class carries the `Class` suffix, the file
  takes the singleton's name instead (`LedClass` lives in `Led.h`), again like
  the Arduino core (`WiFiClass` in `WiFi.h`).

## Style

- Code, identifiers and log strings in **English**.
- No comments — names must reveal intent (`heaterRelay`, not `obj`).
- Class sections in order: `public:` (constructor first), `protected:`,
  `private:` (attributes before methods). Definitions in the `.cpp` follow the
  declaration order of the header.
- Separate a body's guard clauses, its work and its result with a blank line.
  Skip it when there is nothing to separate: a straight-line body, or a single
  statement closing a block (a guard or a loop).
- `const` on methods that do not modify state, and on locals never reassigned.
- Prefer early return and the ternary over nested `if/else`.
- Never return a value no caller consumes: if every call site ignores it, the
  method is `void` and logs its own failure.

## Dependencies

Pinned by version in `platformio.ini`. Do not add, remove or update one without
explicit approval.

## Documentation

Keep `README.md` (the product) and `CLAUDE.md` (how to work in the repo) current
in the same change set, without overlap between them.

The `README.md` **TODO list is the user's**: the only edit allowed on your own
initiative is ticking `[ ]` → `[x]`. Never reword, reorder, renumber, add or
drop an item unless asked.
