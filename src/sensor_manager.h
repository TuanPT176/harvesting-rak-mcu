#ifndef sensor_manager_h
#define sensor_manager_h

#include <Arduino.h>
#include "Adafruit_SHTC3.h"
#include "config.h"

#define USE_RAK3172 0   // RAK3172 = 1, RAK11720 = 0

#if USE_RAK3172
#include "Adafruit_LTR329_LTR303.h"
#endif

struct SensorData {
  int16_t temp;
  uint8_t humi;
  float vbat;
  float lux;
};

class SensorManager {
public:
  void begin();
  SensorData read();
  void SensorWake();
  void SensorSleep();

private:
  Adafruit_SHTC3 shtc3;
  #if USE_RAK3172
  Adafruit_LTR303 ltr;

  float getLux(uint16_t ch0, uint16_t ch1);
  float calibrateLux(float lux_raw);
#endif
};

#endif