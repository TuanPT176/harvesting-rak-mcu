#include "scheduler.h"

Decision makeDecision(float vbat, uint16_t current) {
  Decision d;

  // 🔹 Default (fallback an toàn)
  d.sendNow = true;
  d.sleepTime = 1200; // 20 phút
  d.state = 0;
  // d.sleepTime = 30;
  // 🔴 CRITICAL
  if (vbat < 2.8) {
    d.sendNow = false;
    d.sleepTime = 3600; // 60 phút
    d.state = 30;
    // d.sleepTime = 30;
  }

  // 🟡 LOW
  else if (vbat < 3.0) {
    if (current < 40) {
      // ban đêm / không harvest
      d.sendNow = true;
      d.sleepTime = 1800; // 30 phút
      d.state = 20;
      // d.sleepTime = 30;
    } else {
      // có nắng nhẹ
      d.sendNow = true;
      d.sleepTime = 1200; // 20 phút
      d.state = 21;
      // d.sleepTime = 30;
    }
  }

  // 🟢 NORMAL
  else if (vbat < 3.3) {
    if (current < 40) {
      d.sendNow = true;
      d.sleepTime = 1200; // 20 phút
      d.state = 10;
      // d.sleepTime = 30;
    } else {
      d.sendNow = true;
      d.sleepTime = 900; // 15 phút
      d.state = 11;
      // d.sleepTime = 30;
    }
  }

  // 🔵 HIGH
  else {
    d.sendNow = true;
    d.sleepTime = 600; // 10 phút
    d.state = 0;
    // d.sleepTime = 30;
  }

  return d;
}