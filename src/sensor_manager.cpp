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
  Wire.begin();
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

  // ADC reading with proper settling time
  #if USE_RAK3172
  // RAK3172 - Original config
  const float ADC_REF = 2.4;        // Reference voltage
  const float ADC_MAX = 4095.0;     // 12-bit ADC
  const float VOLTAGE_DIVIDER = 3.0;
  #else
  // RAK11720 - Calibrated config
  // Calibration: ADC_raw=1886 -> VADC_pin=0.91V -> Vsource=2.73V
  // Divider: R_top=2k, R_bottom=1k -> ratio=3.0
  // Vref actual = 1.97V (calculated from: 0.91 * 4095 / 1886)
  // Note: Vref deviation due to internal regulator variation and component tolerance
  const float ADC_REF = 1.97;        // Actual reference voltage
  const float ADC_MAX = 4095.0;      // 12-bit ADC
  const float VOLTAGE_DIVIDER = 3.0; // (2k+1k)/1k
  #endif
  
  // Wait for ADC settling after potential wake
  delay(500);
  
  // Read ADC with retry if value seems invalid
  int adc_value = analogRead(VBAT_PIN);
  if (adc_value < 50) {
    // Value too low, likely transient - wait and retry
    delay(500);
    adc_value = analogRead(VBAT_PIN);
  }
  
  // Calculate battery voltage: V = (ADC_raw/4095) * Vref * divider_ratio
  float voltage = (adc_value / ADC_MAX) * ADC_REF * VOLTAGE_DIVIDER;
  
  Serial.print("VBAT_PIN: ");
  Serial.print(VBAT_PIN);
  Serial.print(" - ADC: ");
  Serial.print(adc_value);
  Serial.print(" - SupCap: ");
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

void SensorManager::SensorSleep() {
  // ===== SHTC3 sleep =====
  Wire.beginTransmission(0x70);
  Wire.write(0xB0);
  Wire.write(0x98);
  Wire.endTransmission();
  #if USE_RAK3172
    ltr.enable(false); 
  #endif

  Serial.println("Sensors → Sleep");
}

void SensorManager::SensorWake() {
  Wire.beginTransmission(0x70);
  Wire.write(0x35);
  Wire.write(0x17);
  Wire.endTransmission();
  delay(1);
  #if USE_RAK3172
    ltr.enable(true);
  #endif
  delay(100);

  Serial.println("Sensors → Wake");
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