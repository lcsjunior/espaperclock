#!/usr/bin/env bash
set -euo pipefail

# Builds data/ into a LittleFS image and flashes it to the board's LittleFS
# partition, bypassing the Arduino IDE's "Upload LittleFS" command.
# CLI equivalent of https://github.com/earlephilhower/arduino-littlefs-upload
#
# Matches the "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)" partition
# scheme. If Tools > Partition Scheme changes in the IDE, update
# PARTITION_OFFSET/PARTITION_SIZE below to match the new partitions.csv.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ARDUINO15="${ARDUINO15:-$HOME/.arduino15}"

PORT="${PORT:-/dev/ttyACM0}"
CHIP="${CHIP:-esp32c3}"
BAUD="${BAUD:-921600}"
PARTITION_OFFSET="${PARTITION_OFFSET:-0x290000}"
PARTITION_SIZE="${PARTITION_SIZE:-0x160000}"

MKLITTLEFS="$(find "$ARDUINO15/packages/esp32/tools/mklittlefs" -maxdepth 2 -type f -name mklittlefs | sort -V | tail -1)"
ESPTOOL="$(find "$ARDUINO15/packages/esp32/tools/esptool_py" -maxdepth 2 -type f -name esptool | sort -V | tail -1)"

if [[ -z "$MKLITTLEFS" ]]; then
  echo "mklittlefs not found under $ARDUINO15/packages/esp32/tools/mklittlefs" >&2
  exit 1
fi
if [[ -z "$ESPTOOL" ]]; then
  echo "esptool not found under $ARDUINO15/packages/esp32/tools/esptool_py" >&2
  exit 1
fi

IMAGE="$(mktemp -t littlefs-XXXXXX.bin)"
trap 'rm -f "$IMAGE"' EXIT

echo "==> Building LittleFS image from data/"
"$MKLITTLEFS" -c "$ROOT_DIR/data" -s "$PARTITION_SIZE" "$IMAGE"

echo "==> Flashing to $PORT at $PARTITION_OFFSET"
"$ESPTOOL" --chip "$CHIP" --port "$PORT" --baud "$BAUD" \
  write-flash "$PARTITION_OFFSET" "$IMAGE"

echo "==> Done"
