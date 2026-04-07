#include "scheduler.h"

Decision makeDecision(float vbat, uint16_t current) {
  Decision d;

  // 🔹 Default (fallback an toàn)
  d.sendNow = false;
  d.sleepTime = 600; // 10 phút

  // 🔴 CRITICAL
  if (vbat < 2.8) {
    d.sendNow = false;
    d.sleepTime = 1800; // 30 phút
  }

  // 🟡 LOW
  else if (vbat < 3.0) {
    if (current < 20) {
      // ban đêm / không harvest
      d.sendNow = false;
      d.sleepTime = 1200; // 20 phút
    } else {
      // có nắng nhẹ
      d.sendNow = true;
      d.sleepTime = 600; // 10 phút
    }
  }

  // 🟢 NORMAL
  else if (vbat < 3.3) {
    if (current < 20) {
      d.sendNow = true;
      d.sleepTime = 300; // 5 phút
    } else {
      d.sendNow = true;
      d.sleepTime = 120; // 2 phút
    }
  }

  // 🔵 HIGH
  else {
    d.sendNow = true;
    d.sleepTime = 300; // 1 phút
  }

  return d;
}