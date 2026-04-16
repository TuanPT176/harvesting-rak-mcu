#include "sensor_manager.h"

#if USE_RAK3172
static float a = 6.35;
static float b = 50;
#endif

void SensorManager::begin() {
  if (!shtc3.begin()) {
    Serial.println("SHTC3 not found");
  } else Serial.println("Found SHTC3 sensor");
  analogReadResolution(12);
  #if USE_RAK3172
  if (!ltr.begin()) {
    Serial.println("LTR303 not found!");
  } else {
    Serial.println("LTR303 OK");

    ltr.setGain(LTR3XX_GAIN_48);
    ltr.setIntegrationTime(LTR3XX_INTEGTIME_100);
    ltr.setMeasurementRate(LTR3XX_MEASRATE_200);
  }
#endif
}

SensorData SensorManager::read() {
  SensorData d;

  sensors_event_t humidity, temp;
  shtc3.getEvent(&humidity, &temp);

  d.temp = temp.temperature * 10;
  d.humi = humidity.relative_humidity;

  int adc_value = analogRead(VBAT_PIN);
  float max = 4095.0;
  float ref = 2.4;
  delay(500);
  delay(500);
  adc_value = analogRead(VBAT_PIN);
  Serial.print(VBAT_PIN);
  Serial.print(" - adc_value ");
  Serial.print(adc_value);
  float voltage = ref * ((float)adc_value / max) * 3;
  Serial.print(" SupCap: ");
  Serial.print(voltage, 3);
  Serial.print("V - ");
  d.vbat = voltage;
  #if USE_RAK3172
  d.lux = 0;
  if (ltr.newDataAvailable()) {
    uint16_t ch0, ch1;

    if (ltr.readBothChannels(ch0, ch1)) {
      float lux_raw = getLux(ch0, ch1);
      // float lux_cal = calibrateLux(lux_raw);
      d.lux = lux_raw;

      Serial.print("Lux: ");
      Serial.println(lux_raw);
    }
  }
  #else
  d.lux = 0;
  #endif
  return d;
}

#if USE_RAK3172
float SensorManager::getLux(uint16_t ch0, uint16_t ch1) {
  float lux = 0.07f * ch0 - 0.02f * ch1;
  if (lux < 0) lux = 0;
  return lux;
}

float SensorManager::calibrateLux(float lux_raw) {
  float lux = a * lux_raw + b;
  return max(0.0f, lux);
}
#endif