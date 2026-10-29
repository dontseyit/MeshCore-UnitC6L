# M5Stack Unit C6L — OLED, button, GPS, dual boot

An alternative to `variants/m5stack_unit_c6l` that uses the whole board:

- **64×48 SSD1306 OLED** (SPI, shares the bus with the SX1262) running the companion `ui-tiny` UI
- **User button** (read through the PI4IOE5V6408 IO expander)
- **RF front end set up properly**: SX1262 reset, antenna enable and RX enable via the expander, TCXO at 3.0 V
  (the stock variant never sets up the expander and runs the TCXO at 1.8 V)
- **Grove GPS** (M5Stack Unit GPS v1.1) on the HY2.0-4P port
- **Dual boot**: companion (BLE) and repeater installed side by side, each with its own storage

## Pins

| Function | Pin |
|---|---|
| SX1262 SCK / MISO / MOSI / NSS | 20 / 22 / 21 / 23 |
| SX1262 DIO1 / BUSY | 7 / 19 |
| OLED CS / DC / RESET | 6 / 18 / 15 |
| Internal I2C (IO expander 0x43) SDA / SCL | 10 / 8 |
| Expander P0 / P5 / P6 / P7 | button / RX enable (off during TX) / antenna enable / SX1262 NRST |
| Buzzer | 11 |
| Grove: GPS TX → C6L / C6L → GPS RX | 4 (white) / 5 (yellow) |

## Environments

| Env | Firmware |
|---|---|
| `M5Stack_Unit_C6L_UI_companion_radio_ble` | Companion for the phone app over Bluetooth (app slot 0) |
| `M5Stack_Unit_C6L_UI_companion_radio_usb` | Companion over USB |
| `M5Stack_Unit_C6L_UI_repeater` | Repeater, CLI on USB serial (app slot 1, `spiffs2` storage) |
| `M5Stack_Unit_C6L_UI_room_server` | Room server (shares the companion's storage, not set up for dual boot) |

## Flashing both firmwares

```sh
variants/m5stack_unit_c6l_ui/flash_dualboot.sh /dev/cu.usbmodemXXXX
```

This writes the companion to slot 0 and the repeater to slot 1, then boots the companion.
A plain `pio run -t upload` only rewrites the companion.

If the build fails with `BLESecurity has no member named 'setStaticPIN'`, your PlatformIO
has a newer arduino-esp32 installed globally; build with a separate core dir, e.g.
`PLATFORMIO_CORE_DIR=$PWD/../.pio-core`.

## Switching firmware

Hold the button while powering on, keep holding ~1 s until the screen says
"release btn", then let go. The device reboots into the other firmware.

## Button

| Gesture | Companion | Repeater |
|---|---|---|
| Click | Next page / wake screen | Wake screen |
| Double-click | Previous page | Send flood advert |
| Triple-click | Toggle buzzer | — |
| Long press | Page action (advert, Bluetooth, GPS, hibernate) | **Power off** |

## Repeater console

The repeater CLI has no line editing, so use the bundled line-based console
(edit, history, Ctrl-D to quit; it doesn't reset the board on open):

```sh
~/.platformio/penv/bin/python variants/m5stack_unit_c6l_ui/console.py /dev/cu.usbmodemXXXX
```

Change the default admin password first: `password <new>`.
