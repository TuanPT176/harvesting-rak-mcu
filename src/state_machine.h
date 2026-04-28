#ifndef state_machine_h
#define state_machine_h

#include <Arduino.h>
#include "config.h"
#include "neh7100.h"
#include "sensor_manager.h"
#include "LoRaWAN.h"
#include "scheduler.h"

// Runtime state machine
enum SystemState {
  STATE_INIT,           // Khởi tạo hệ thống
  STATE_COLLECT_DECIDE, // Thu thập dữ liệu & đưa ra quyết định
  STATE_TRANSMIT,       // Gửi dữ liệu
  STATE_SLEEP           // Ngủ
};

class StateMachine {
public:
  StateMachine();
  void init(NEH7100 *pmic, SensorManager *sensor, LoRaWAN *lora);
  void update();
  SystemState getCurrentState() { return currentState; }
  
private:
  SystemState currentState;
  NEH7100 *pmic_ptr;
  SensorManager *sensor_ptr;
  LoRaWAN *lora_ptr;
  
  // State variables
  uint32_t tx_start_time;
  SensorData last_sensor;
  Decision last_decision;
  uint16_t last_current_x10;
  
  // State handlers
  void handleStateInit();
  void handleStateCollectDecide();
  void handleStateTransmit();
  void handleStateSleep();
  
  // Helper functions
  void transitionTo(SystemState newState);
  void printStateChange(SystemState oldState, SystemState newState);
};

#endif
