#include "neh7100.h"

void NEH7100::begin() {
  Wire.begin();
}

uint8_t NEH7100::readRegister(uint8_t regAddr) {
  Wire.beginTransmission(address);
  Wire.write(regAddr);
  Wire.endTransmission();

  Wire.requestFrom(address, (uint8_t)1);
  if (Wire.available()) return Wire.read();

  return 0;
}

void NEH7100::readAll() {
  for (int i = 0; i < 11; i++) {
    reg[i] = readRegister(i);
  }
}

void NEH7100::writeRegister(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(val);
  uint8_t status = Wire.endTransmission();
  if (status == 0) {
    Serial.print("Ghi OK -> Reg 0x");
    Serial.print(reg, HEX);
    Serial.print(" = 0x");
    Serial.println(reg, HEX);
  } else {
    Serial.print("Ghi FAIL tại Reg 0x");
    Serial.println(reg, HEX);
  }
}

void NEH7100::writeAllConfigs() {
  writeRegister(0x01, 0x67);
  delay(10);
  writeRegister(0x04, 0x20);
  delay(10);
  writeRegister(0x05, 0x06);
  delay(10);
}

uint16_t NEH7100::getCurrent_uA_x10() {
  uint8_t i_range = reg[0x09] & 0x03;
  uint8_t val = reg[0x0A];

  uint32_t current = 0;

  switch (i_range) {
    case 0: current = (val * 706) / 1000; break;  // 0.0706 µA
    case 1: current = (val * 478) / 100; break;   // 0.478 µA
    case 2: current = (val * 471) / 10; break;    // 4.71 µA
    case 3: current = val * 675; break;           // 67.5 µA
  }

  return (uint16_t)current;
}

void NEH7100::ensureConfig() {
  readAll();

  if (reg[0x01] != REG01_EXPECTED) {
    writeRegister(0x01, REG01_EXPECTED);
    delay(10);
  }

  if (reg[0x04] != REG04_EXPECTED) {
    writeRegister(0x04, REG04_EXPECTED);
    delay(10);
  }

  if (reg[0x05] != REG05_EXPECTED) {
    writeRegister(0x05, REG05_EXPECTED);
    delay(10);
  }
}
