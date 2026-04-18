#define USE_RAK3172 1

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

  sensor.SensorWake();
  pmic.readAll();
  current_x10 = pmic.getCurrent_uA_x10();
  pmic.ensureConfig();
  pmic.updateFrequency(current_x10);
  g_sensor = sensor.read();

  d = makeDecision(g_sensor.vbat, current_x10);
  Serial.print("State: ");
  Serial.println(d.state);

// Nếu đang gửi → không làm gì, chờ TX_DONE
    if (tx_pending)
    {
        if (tx_done)
        {
            Serial.println("TX done → go sleep");
            tx_done = false;
            tx_pending = false;

            lora.sleep(d.sleepTime);
        }
        return;
    }

    // Nếu cần gửi
    if (d.sendNow)
    {
        tx_done = false;
        if (lora.uplink_routine())
        {
            tx_pending = true;
        }
        lora.sleep(d.sleepTime);
    }
    else
    {
        lora.sleep(d.sleepTime);
    }
}