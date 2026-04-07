#ifndef neh7100_h
#define neh7100_h
#define REG01_EXPECTED 0x67
#define REG04_EXPECTED 0x20
#define REG05_EXPECTED 0x06

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "sensor_manager.h"

extern SensorData g_sensor;
extern uint8_t collected_data[64];
extern uint16_t current_x10;

class NEH7100 {
public:
  void begin();
  void readAll();
  void writeAllConfigs();
  uint16_t getCurrent_uA_x10();
  void ensureConfig();
  uint8_t reg[11];
  uint16_t current_x10;

private:
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t val);

  uint8_t address = NEH7100_ADDR;
};

#endif