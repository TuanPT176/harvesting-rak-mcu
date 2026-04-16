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

  if (tx_pending) {
        return;
    }
  pmic.readAll();
  current_x10 = pmic.getCurrent_uA_x10();
  pmic.ensureConfig();
  pmic.updateFrequency(current_x10);
  g_sensor = sensor.read();

  d = makeDecision(g_sensor.vbat, current_x10);
  Serial.print("State: ");
  Serial.println(d.state);
    if (d.sendNow && !tx_pending) {
        tx_pending = true;
        tx_done = false;
        lora.uplink_routine();
    }
    else if (!tx_pending) {
    lora.sleep(d.sleepTime);
    }

    if (tx_done) {
        tx_done = false;
        tx_pending = false;

        lora.sleep(d.sleepTime);
    }
}