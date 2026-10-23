#include <Arduino.h>
#include "target.h"
#include <esp_ota_ops.h>

UnitC6LBoard board;

// global SPI: same host as the OLED, started in board.begin()
RADIO_CLASS radio = new Module(P_LORA_NSS, P_LORA_DIO_1, P_LORA_RESET, P_LORA_BUSY, SPI);

WRAPPER_CLASS radio_driver(radio, board);

ESP32RTCClock fallback_clock;
AutoDiscoverRTCClock rtc_clock(fallback_clock);
#if ENV_INCLUDE_GPS
  #include <helpers/sensors/MicroNMEALocationProvider.h>
  MicroNMEALocationProvider nmea = MicroNMEALocationProvider(Serial1, &rtc_clock);  // GPS on the Grove port
  EnvironmentSensorManager sensors = EnvironmentSensorManager(nmea);
#else
  EnvironmentSensorManager sensors;
#endif

#ifdef DISPLAY_CLASS
  DISPLAY_CLASS display;
  ExpanderButton user_btn(board);
#endif

void UnitC6LBoard::checkFirmwareSwitch() {
  for (int i = 0; i < 50; i++) {   // must be held ~1s
    if (!isUserButtonDown()) return;
    delay(20);
  }
  const esp_partition_t* next = esp_ota_get_next_update_partition(NULL);
  esp_app_desc_t desc;
  bool ok = next && esp_ota_get_partition_description(next, &desc) == ESP_OK;
#ifdef DISPLAY_CLASS
  display.begin();
  display.startFrame();
  display.drawTextCentered(display.width() / 2, 14, ok ? "Switching" : "No other");
  display.drawTextCentered(display.width() / 2, 24, ok ? "firmware.." : "firmware");
  display.drawTextCentered(display.width() / 2, 34, "release btn");
  display.endFrame();
#endif
  while (isUserButtonDown()) delay(20);   // else the other firmware switches straight back
  if (ok && esp_ota_set_boot_partition(next) == ESP_OK) ESP.restart();
}

bool radio_init() {
  fallback_clock.begin();
  rtc_clock.begin(Wire);
  return radio.std_init(&SPI);
}

mesh::LocalIdentity radio_new_identity() {
  RadioNoiseListener rng(radio);
  return mesh::LocalIdentity(&rng);  // create new random identity
}
