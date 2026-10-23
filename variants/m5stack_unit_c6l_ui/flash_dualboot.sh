#!/bin/sh
# Flash companion (BLE) into app0 and repeater into app1, boot the companion.
# Hold the button at power-on to switch between them.
# Usage: variants/m5stack_unit_c6l_ui/flash_dualboot.sh <serial-port>
set -e
PORT=${1:?usage: $0 <serial-port>}
cd "$(dirname "$0")/../.."
export PLATFORMIO_CORE_DIR="${PLATFORMIO_CORE_DIR:-$(cd .. && pwd)/.pio-core}"
PIO="${PIO:-$HOME/.platformio/penv/bin/pio}"
APP1_OFFSET=0x1F0000   # must match partitions_dualboot.csv

"$PIO" run -e M5Stack_Unit_C6L_UI_repeater
"$PIO" run -e M5Stack_Unit_C6L_UI_companion_radio_ble -t upload --upload-port "$PORT"
"$PIO" pkg exec -p tool-esptoolpy -- esptool.py --chip esp32c6 --port "$PORT" \
  write_flash $APP1_OFFSET .pio/build/M5Stack_Unit_C6L_UI_repeater/firmware.bin
