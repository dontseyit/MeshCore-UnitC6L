#!/usr/bin/env python3
"""Line-based console for the MeshCore CLI (repeater / room server).

The firmware takes every key literally (no backspace), so lines are edited here
and only sent on Enter. Arrow keys and history work. Ctrl-D or Ctrl-C quits.
On connect it sets the device clock from this computer (the firmware only lets it move forward).

Usage: ~/.platformio/penv/bin/python console.py <serial-port> [baud]
"""
import readline  # noqa: F401  (enables line editing + history for input())
import sys
import threading
import time

import serial


def reader(ser):
    try:
        while True:
            data = ser.read(256)
            if data:
                sys.stdout.write(data.decode(errors="replace"))
                sys.stdout.flush()
    except Exception:  # port closed on exit
        pass


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    ser = serial.Serial()
    ser.port = sys.argv[1]
    ser.baudrate = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    ser.timeout = 0.1
    ser.rts = False  # RTS without DTR resets the ESP32-C6, so keep DTR and drop only RTS
    ser.open()
    threading.Thread(target=reader, args=(ser,), daemon=True).start()
    print(f"Connected to {ser.port}. Type a command, Enter to send, Ctrl-D to quit.")
    ser.write(f"time {int(time.time())}\r".encode())  # no battery-backed RTC on the C6L
    try:
        while True:
            ser.write(input().encode() + b"\r")
    except (EOFError, KeyboardInterrupt):
        print()
    finally:
        ser.close()


if __name__ == "__main__":
    main()
