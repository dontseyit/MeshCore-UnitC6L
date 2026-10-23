#pragma once

#define RADIOLIB_STATIC_ONLY 1
#include <RadioLib.h>
#include <UnitC6LBoard.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#include <helpers/radiolib/CustomSX1262Wrapper.h>
#include <helpers/AutoDiscoverRTCClock.h>
#include <helpers/sensors/EnvironmentSensorManager.h>
#ifdef DISPLAY_CLASS
  #include <helpers/ui/U8g2Display.h>
#endif

extern UnitC6LBoard board;
extern WRAPPER_CLASS radio_driver;
extern AutoDiscoverRTCClock rtc_clock;
extern EnvironmentSensorManager sensors;

#ifdef DISPLAY_CLASS
  extern DISPLAY_CLASS display;
  extern ExpanderButton user_btn;
#endif

bool radio_init();
mesh::LocalIdentity radio_new_identity();
