#define USE_RAK3172 0

#include "src/config.h"
#include "src/neh7100.h"
#include "src/sensor_manager.h"
#include "src/LoRaWAN.h"
#include "src/scheduler.h"



NEH7100 pmic;
SensorManager sensor;
LoRaWAN lora;

uint8_t collected_data[64];
SensorData g_sensor;
Decision d;
uint16_t current_x10;
uint32_t lastCheck = 0;
bool tx_done;

void setup() {
  Serial.begin(115200, RAK_AT_MODE);
  Serial.println("=========RESET=========");
  lora.begin();
  pmic.begin();
  pmic.writeAllConfigs();
  sensor.begin();
  
}

void loop() {
  static bool tx_pending = false;
  static uint32_t tx_start_time = 0;

  // ================================
  // 🔴 CASE 1: ĐANG CHỜ TX → TIMEOUT GUARD
  // ================================
  if (tx_pending)
  {
    // Timeout protection: nếu chờ TX quá 30 giây → force reset
    // (Callback LoRa có thể trễ, nên timeout đủ dài)
    uint32_t elapsed = millis() - tx_start_time;
    
    if (elapsed > 30000)
    {
      Serial.println("\r\n[ERROR] TX timeout > 30s, forcing sleep\r\n");
      Serial.flush();
      tx_pending = false;
      tx_done = false;
      sensor.SensorSleep();
      delay(100);
      lora.sleep(d.sleepTime);
      return;
    }

    if (tx_done)
    {
      Serial.println("\r\n[SUCCESS] TX done, going to sleep\r\n");
      Serial.flush();
      
      tx_done = false;
      tx_pending = false;
      
      sensor.SensorSleep();
      delay(100);
      lora.sleep(d.sleepTime);
      return;
    }
    else
    {
      delay(1000);
      return;
    }
  }

  // ================================
  // 🟢 CASE 2: BÌNH THƯỜNG - ĐỌC SENSOR VÀ QUYẾT ĐỊNH
  // ================================
  sensor.SensorWake();
  pmic.readAll();
  current_x10 = pmic.getCurrent_uA_x10();
  pmic.ensureConfig();
  pmic.updateFrequency(current_x10);
  delay(100);
  g_sensor = sensor.read();

  d = makeDecision(g_sensor.vbat, current_x10);
  Serial.print("State: ");
  Serial.println(d.state);
  Serial.flush();

  // ================================
  // 🟡 CASE 3: KHÔNG CẦN GỬI → NGỦ NGAY
  // ================================
  if (!d.sendNow)
  {
    Serial.println("No need to send → go sleep");
    Serial.flush();
    
    sensor.SensorSleep();
    delay(100);
    lora.sleep(d.sleepTime);
    return;
  }

  // ================================
  // 🟠 CASE 4: CẦN GỬI → BẮT ĐẦU TRUYỀN
  // ================================
  Serial.println("Start transmission...");
  Serial.flush();
  
  tx_done = false;
  
  Serial.println("DEBUG: Before uplink_routine()");
  Serial.flush();
  
  bool uplink_result = lora.uplink_routine();
  
  Serial.print("DEBUG: After uplink_routine(), result=");
  Serial.println(uplink_result ? "TRUE" : "FALSE");
  Serial.flush();

  if (uplink_result)
  {
    Serial.println("TX requested → waiting for callback");
    Serial.flush();
    
    tx_pending = true;
    tx_start_time = millis();
  }
  else
  {
    Serial.println("Send failed → go sleep");
    Serial.flush();
    
    sensor.SensorSleep();
    delay(100);
    lora.sleep(d.sleepTime);
  }
}