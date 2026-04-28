#define USE_RAK3172 0

#include "src/config.h"
#include "src/neh7100.h"
#include "src/sensor_manager.h"
#include "src/LoRaWAN.h"
#include "src/scheduler.h"
#include "src/state_machine.h"



NEH7100 pmic;
SensorManager sensor;
LoRaWAN lora;
StateMachine state_machine;

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
  
  // Initialize state machine
  state_machine.init(&pmic, &sensor, &lora);
}

void loop() {
  // Run state machine
  state_machine.update();
}