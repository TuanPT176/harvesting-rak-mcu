#ifndef sensor_manager_h
#define sensor_manager_h

#include <Arduino.h>
#include "Adafruit_SHTC3.h"
#include "config.h"

struct SensorData {
  int16_t temp;
  uint8_t humi;
  uint16_t vbat;
};

class SensorManager {
public:
  void begin();
  SensorData read();

private:
  Adafruit_SHTC3 shtc3;
};

#endif