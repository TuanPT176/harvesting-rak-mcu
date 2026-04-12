#ifndef neh7100_h
#define neh7100_h
#define REG00_EXPECTED 0x48   
#define REG01_EXPECTED 0x67
#define REG04_EXPECTED 0x20
#define REG05_EXPECTED 0x06
#define FREQ_32K   0
#define FREQ_64K   1
#define FREQ_128K  2
#define FREQ_256K  3
#define FREQ_512K  4
#define FREQ_1M    5

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
  void setFrequency(uint8_t fmax, uint8_t fmin);
  void updateFrequency(uint16_t current_uA);
  uint8_t reg[11];
  uint16_t current_x10;

private:
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t val);

  uint8_t address = NEH7100_ADDR;
};

#endif