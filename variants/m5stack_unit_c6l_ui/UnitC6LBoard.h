#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <helpers/ESP32Board.h>

// PI4IOE5V6408 IO expander on the internal I2C bus (PIN_BOARD_SDA/SCL)
#define PI4IO_ADDR          0x43
#define PI4IO_REG_IO_DIR    0x03   // 1 = output
#define PI4IO_REG_OUT_SET   0x05
#define PI4IO_REG_OUT_HIZ   0x07   // 1 = output high-impedance
#define PI4IO_REG_PULL_EN   0x0B
#define PI4IO_REG_PULL_SEL  0x0D   // 1 = pull-up
#define PI4IO_REG_IN_STA    0x0F

#define PI4IO_BTN       (1 << 0)   // user button, active low
#define PI4IO_RX_EN     (1 << 5)   // receive path / LNA; off while transmitting (Zephyr: rx-enable-gpios)
#define PI4IO_ANT_SW    (1 << 6)   // antenna enable
#define PI4IO_LORA_RST  (1 << 7)   // SX1262 NRST, active low

class UnitC6LBoard : public ESP32Board {
  uint8_t _inputs = 0xFF;
  unsigned long _inputs_at = 0;

  void ioWrite(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(PI4IO_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
  }

public:
  void begin() {
    ESP32Board::begin();

    // SX1262 and OLED share one SPI bus: deselect both before anything talks on it
    pinMode(P_LORA_NSS, OUTPUT);
    digitalWrite(P_LORA_NSS, HIGH);
    pinMode(PIN_OLED_CS, OUTPUT);
    digitalWrite(PIN_OLED_CS, HIGH);
    SPI.begin(P_LORA_SCLK, P_LORA_MISO, P_LORA_MOSI);  // later begin() calls are no-ops

    const uint8_t outs = PI4IO_RX_EN | PI4IO_ANT_SW | PI4IO_LORA_RST;
    ioWrite(PI4IO_REG_OUT_SET, 0);              // radio in reset, RF path off
    ioWrite(PI4IO_REG_IO_DIR, outs);
    ioWrite(PI4IO_REG_OUT_HIZ, (uint8_t)~outs);
    ioWrite(PI4IO_REG_PULL_SEL, PI4IO_BTN);     // button pulled up, other inputs pulled down
    ioWrite(PI4IO_REG_PULL_EN, (uint8_t)~outs);
    delay(100);
    ioWrite(PI4IO_REG_OUT_SET, outs);           // release reset, antenna + RX path on

    checkFirmwareSwitch();
  }

  // TX enable is DIO2; the RX path must be off while transmitting
  void onBeforeTransmit() override { ioWrite(PI4IO_REG_OUT_SET, PI4IO_ANT_SW | PI4IO_LORA_RST); }
  void onAfterTransmit() override { ioWrite(PI4IO_REG_OUT_SET, PI4IO_RX_EN | PI4IO_ANT_SW | PI4IO_LORA_RST); }

  void checkFirmwareSwitch();   // button held at power-on -> boot the other app slot

  bool isUserButtonDown() {
    // ponytail: polled over I2C with a 10ms cache; use the expander INT line if latency matters
    if (millis() - _inputs_at >= 10) {
      Wire.beginTransmission(PI4IO_ADDR);
      Wire.write(PI4IO_REG_IN_STA);
      if (Wire.endTransmission(false) == 0 && Wire.requestFrom((uint8_t)PI4IO_ADDR, (uint8_t)1) == 1) {
        _inputs = Wire.read();
      }
      _inputs_at = millis();
    }
    return !(_inputs & PI4IO_BTN);
  }

  const char* getManufacturerName() const override {
    return "Unit C6L";
  }
};

#ifdef DISPLAY_CLASS
#include <helpers/ui/MomentaryButton.h>

class ExpanderButton : public MomentaryButton {
  UnitC6LBoard& _board;
public:
  ExpanderButton(UnitC6LBoard& board) : MomentaryButton(0, 1000, true), _board(board) {}
  void begin() override { }   // not a GPIO; pin 0 is only a placeholder
protected:
  int readRaw() const override { return _board.isUserButtonDown() ? LOW : HIGH; }
};
#endif
