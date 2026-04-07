#ifndef scheduler_h
#define scheduler_h

#include <Arduino.h>
#include "config.h"

struct Decision {
  bool sendNow;
  uint32_t sleepTime;
};

Decision makeDecision(float vbat, uint16_t current);

#endif