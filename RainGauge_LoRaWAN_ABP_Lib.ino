#include "src/config.h"
#include "src/neh7100.h"
#include "src/sensor_manager.h"
#include "LoRaWAN.h"
#include "scheduler.h"

NEH7100 pmic;
SensorManager sensor;
LoRaWAN lora;

uint8_t collected_data[64];
SensorData g_sensor;
Decision d;
uint16_t current_x10;
uint32_t lastCheck = 0;
void setup() {
  Serial.begin(115200, RAK_AT_MODE);
  Serial.println("=========RESET=========");
  lora.begin();
  pmic.begin();
  pmic.writeAllConfigs();
  sensor.begin();
  
}

void loop() {

  pmic.readAll();
  current_x10 = pmic.getCurrent_uA_x10();
  pmic.ensureConfig();
  pmic.updateFrequency(current_x10);
  g_sensor = sensor.read();

  d = makeDecision(g_sensor.vbat, current_x10);
  Serial.print("State: ");
  Serial.println(d.state);
  if (d.sendNow) {
    lora.uplink_routine();
    delay(200); // đảm bảo TX done
  }
   lora.sleep(d.sleepTime);

  
}