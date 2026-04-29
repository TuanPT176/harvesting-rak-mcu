#include "state_machine.h"

extern uint8_t collected_data[64];
extern SensorData g_sensor;
extern Decision d;
extern uint16_t current_x10;
extern bool tx_done;

StateMachine::StateMachine() : currentState(STATE_INIT), tx_start_time(0) {
  pmic_ptr = nullptr;
  sensor_ptr = nullptr;
  lora_ptr = nullptr;
}

void StateMachine::init(NEH7100 *pmic, SensorManager *sensor, LoRaWAN *lora) {
  pmic_ptr = pmic;
  sensor_ptr = sensor;
  lora_ptr = lora;
  currentState = STATE_INIT;
  Serial.println("[SM] State Machine initialized");
}

void StateMachine::update() {
  switch (currentState) {
    case STATE_INIT:
      handleStateInit();
      break;
    case STATE_COLLECT_DECIDE:
      handleStateCollectDecide();
      break;
    case STATE_TRANSMIT:
      handleStateTransmit();
      break;
    case STATE_SLEEP:
      handleStateSleep();
      break;
    default:
      transitionTo(STATE_INIT);
      break;
  }
}

void StateMachine::handleStateInit() {
  // Init → collect & decide
  Serial.println("[SM] Initialization complete, starting data collection");
  Serial.flush();
  transitionTo(STATE_COLLECT_DECIDE);
}

void StateMachine::handleStateCollectDecide() {
  // ============ THU THẬP DỮ LIỆU ============
  sensor_ptr->SensorWake();
  pmic_ptr->readAll();
  last_current_x10 = pmic_ptr->getCurrent_uA_x10();
  pmic_ptr->ensureConfig();
  pmic_ptr->updateFrequency(last_current_x10);
  delay(100);
  
  last_sensor = sensor_ptr->read();
  
  Serial.print("[SM] Sensor - Vbat: ");
  Serial.print(last_sensor.vbat, 3);
  Serial.print("V, Current: ");
  Serial.println(last_current_x10);
  Serial.flush();
  
  // ============ ĐƯA RA QUYẾT ĐỊNH ============
  last_decision = makeDecision(last_sensor.vbat, last_current_x10);
  
  Serial.print("[SM] Decision - State: ");
  Serial.print(last_decision.state);
  Serial.print(", Send: ");
  Serial.println(last_decision.sendNow ? "YES" : "NO");
  Serial.flush();
  
  // Update global vars for LoRa
  g_sensor = last_sensor;
  d = last_decision;
  current_x10 = last_current_x10;
  
  // Collect→Decide → Transmit or Sleep
  if (!last_decision.sendNow) {
    Serial.println("[SM] No transmission needed");
    transitionTo(STATE_SLEEP);
  } else {
    transitionTo(STATE_TRANSMIT);
  }
}

void StateMachine::handleStateTransmit() {
  // ============ TRạNG THÁI TRUYỀN ĐẦU TIÊN ============
  if (tx_start_time == 0) {
    // Lần đầu vào state này - khởi tạo transmit
    Serial.println("[SM] Starting transmission...");
    Serial.flush();
    
    tx_done = false;
    bool uplink_result = lora_ptr->uplink_routine();
    
    Serial.print("[SM] uplink_routine() = ");
    Serial.println(uplink_result ? "TRUE" : "FALSE");
    Serial.flush();
    
    if (!uplink_result) {
      // Transmit failed
      Serial.println("[SM] TX failed, going to sleep");
      Serial.flush();
      tx_start_time = 0;
      transitionTo(STATE_SLEEP);
      return;
    }
    
    // Transmit started - set timer
    Serial.println("[SM] TX requested, waiting for callback");
    Serial.flush();
    tx_start_time = millis();
  }
  
  // ============ CHỜ TX CALLBACK ============
  uint32_t elapsed = millis() - tx_start_time;
  
  if (elapsed > 30000) {
    Serial.println("\r\n[SM ERROR] TX timeout > 30s\r\n");
    Serial.flush();
    tx_done = false;
    tx_start_time = 0;
    transitionTo(STATE_SLEEP);
    return;
  }
  
  if (tx_done) {
    Serial.println("\r\n[SM SUCCESS] TX done, preparing to sleep\r\n");
    Serial.flush();
    tx_done = false;
    tx_start_time = 0;
    transitionTo(STATE_SLEEP);
    return;
  }
  
  // Still waiting - feed watchdog
  delay(1000);
}

void StateMachine::handleStateSleep() {
  // Prepare for sleep
  Serial.println("[SM] Entering sleep mode");
  Serial.flush();
  
  sensor_ptr->SensorSleep();
  delay(100);
  
  lora_ptr->sleep(last_decision.sleepTime);
  
  // After sleep, return to IDLE
  transitionTo(STATE_COLLECT_DECIDE);
}

void StateMachine::transitionTo(SystemState newState) {
  if (newState != currentState) {
    printStateChange(currentState, newState);
    currentState = newState;
  }
}

void StateMachine::printStateChange(SystemState oldState, SystemState newState) {
  Serial.print("[SM] ");
  
  const char* oldName = "";
  const char* newName = "";
  
  switch (oldState) {
    case STATE_INIT: oldName = "INIT"; break;
    case STATE_COLLECT_DECIDE: oldName = "COLLECT_DECIDE"; break;
    case STATE_TRANSMIT: oldName = "TRANSMIT"; break;
    case STATE_SLEEP: oldName = "SLEEP"; break;
  }
  
  switch (newState) {
    case STATE_INIT: newName = "INIT"; break;
    case STATE_COLLECT_DECIDE: newName = "COLLECT_DECIDE"; break;
    case STATE_TRANSMIT: newName = "TRANSMIT"; break;
    case STATE_SLEEP: newName = "SLEEP"; break;
  }
  
  Serial.print(oldName);
  Serial.print(" → ");
  Serial.println(newName);
  Serial.flush();
}
