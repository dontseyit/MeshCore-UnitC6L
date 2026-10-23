# MeshCore for the M5Stack Unit C6L

Unofficial fork of [MeshCore](https://github.com/meshcore-dev/MeshCore) that makes full use of the
M5Stack Unit C6L (ESP32-C6 + SX1262, 64×48 OLED). Apart from one new board variant and a few small,
default-preserving hooks in shared code, this is upstream MeshCore unchanged.

## What it adds over the stock `m5stack_unit_c6l` variant

- **OLED UI**: MeshCore's compact companion UI on the 64×48 screen, fonts sized for it, 60 s screen timeout
- **Button**: page through screens, send adverts, toggle Bluetooth/GPS
- **RF setup**: initializes the on-board IO expander that drives the SX1262 reset, antenna switch and LNA
  (the stock variant never sets it up), and frees the pins it misused (OLED reset, Grove)
- **Grove GPS**: M5Stack Unit GPS v1.1 on the HY2.0-4P port
- **Dual boot**: companion (phone app over Bluetooth) and repeater installed side by side, each with its
  own identity and settings. Hold the button at power-on to switch.

Tested on a Unit C6L: Bluetooth companion, OLED, button, GPS, repeater and switching between them.
The USB companion and room-server builds compile but haven't been tested on hardware.

## Quick start

Requires [PlatformIO](https://platformio.org/).

```sh
git clone https://github.com/dontseyit/MeshCore-UnitC6L.git
cd MeshCore-UnitC6L
variants/m5stack_unit_c6l_ui/flash_dualboot.sh /dev/cu.usbmodemXXXX   # Linux: /dev/ttyACM0
```

The script builds with its own PlatformIO core in `../.pio-core` so its pinned ESP32 toolchain can't
clash with other projects; the first run downloads it. Coming from the stock MeshCore C6L firmware,
the companion keeps its storage location, so your identity and contacts carry over.

Pins, button gestures, firmware switching and the repeater console are documented in the
[variant README](/variants/m5stack_unit_c6l_ui/README.md).

## Staying in sync

The `c6l-ui` branch is kept rebased on upstream MeshCore releases. Generally useful parts are
intended to be offered upstream.

## License

MIT, same as MeshCore. See [license.txt](/license.txt).
