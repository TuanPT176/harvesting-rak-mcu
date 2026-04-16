#include "sensor_manager.h"

#if USE_LTR303
static float a = 6.35;
static float b = 50;
#endif

void SensorManager::begin() {
  if (!shtc3.begin()) {
    Serial.println("SHTC3 not found");
  } else Serial.println("Found SHTC3 sensor");
  analogReadResolution(12);
  #if USE_LTR303
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
  #if USE_LTR303
  d.lux = 0;
  if (ltr.newDataAvailable()) {
    uint16_t ch0, ch1;

    if (ltr.readBothChannels(ch0, ch1)) {
      float lux_raw = getLux(ch0, ch1);
      float lux_cal = calibrateLux(lux_raw);
      d.lux = lux_cal;

      Serial.print("Lux: ");
      Serial.println(lux_cal);
    }
  }
  #else
  d.lux = 0;
  #endif
  return d;
}

#if USE_LTR303
float SensorManager::getLux(uint16_t ch0, uint16_t ch1) {
  if (ch0 == 0) return 0;

  float ratio = (float)ch1 / ch0;
  float lux = 0;

  if (ratio < 0.5) {
    lux = (0.0304 * ch0) - (0.062 * ch0 * pow(ratio, 1.4));
  } else if (ratio < 0.61) {
    lux = (0.0224 * ch0) - (0.031 * ch1);
  } else if (ratio < 0.80) {
    lux = (0.0128 * ch0) - (0.0153 * ch1);
  } else if (ratio < 1.30) {
    lux = (0.00146 * ch0) - (0.00112 * ch1);
  } else {
    lux = 0;
  }
  // normalize gain = 48
  lux = lux / 48.0;

  return lux;
}

float SensorManager::calibrateLux(float lux_raw) {
  float lux = a * lux_raw + b;
  return max(0.0f, lux);
}
#endif