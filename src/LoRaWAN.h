#ifndef LoRaWAN_h
#define LoRaWAN_h

#include <Arduino.h>
#include "config.h"
#include "sensor_manager.h"
#include "scheduler.h"

extern uint8_t minuteTimeSleep;
extern uint32_t downlinkTimeSleep;

extern SensorManager sensor;
extern SensorData g_sensor;
extern uint8_t collected_data[64];
extern uint16_t current_x10;
extern Decision d;

class LoRaWAN {
public:
  
  
  void begin();
  bool uplink_routine();
  static void recvCallback(SERVICE_LORA_RECEIVE_T * data);
  static void sendCallback(int32_t status);
  void sleep(uint32_t sleeptime);

private:
  uint32_t estimatedNextUplink = 0;
};

#endif