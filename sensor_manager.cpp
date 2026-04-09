#include "sensor_manager.h"

void SensorManager::begin() {
  if (!shtc3.begin()) {
    Serial.println("SHTC3 not found");
  } else Serial.println("Found SHTC3 sensor");
  analogReadResolution(14);
}

SensorData SensorManager::read() {
  SensorData d;

  sensors_event_t humidity, temp;
  shtc3.getEvent(&humidity, &temp);

  d.temp = temp.temperature * 10;
  d.humi = humidity.relative_humidity;

  int adc_value = analogRead(VBAT_PIN);
  float max = 16384.0;
  float ref = 2.4;
  delay(500);
  adc_value = analogRead(VBAT_PIN);
  Serial.print("adc_value: ");
  Serial.println(adc_value);
  float voltage = ref * ((float)adc_value / max) * 2.5;
  Serial.print("SupCap: ");
  Serial.print(voltage, 3);
  Serial.print("V - ");
  d.vbat = voltage;
  Serial.print("SupCap: ");
  Serial.print(d.vbat, 3);
  Serial.println("V");



  return d;
}