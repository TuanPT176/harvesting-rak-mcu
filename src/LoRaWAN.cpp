// #include "LoRaWAN.h"

// extern Decision d;
// extern SensorData g_sensor;
// extern uint16_t current_x10;
// extern uint8_t collected_data[];

// extern bool tx_done;

// // ================= CALLBACK =================
// void LoRaWAN::recvCallback(SERVICE_LORA_RECEIVE_T *data)
// {
//     if (data->BufferSize > 0) {
//         Serial.println("Downlink received:");

//         for (int i = 0; i < data->BufferSize; i++) {
//             Serial.printf("%02X ", data->Buffer[i]);
//         }
//         Serial.println();

//         uint8_t minuteTimeSleep = data->Buffer[0];
//         uint32_t downlinkTimeSleep = minuteTimeSleep * 60000;

//         Serial.print("Sleep from DL: ");
//         Serial.println(downlinkTimeSleep);
//     }
// }

// void LoRaWAN::sendCallback(int32_t status)
// {
//     if (status == 0) {
//         Serial.println("Send OK");
//     } else {
//         Serial.println("Send FAIL");
//     }

//     tx_done = true;   // 🔥 QUAN TRỌNG
// }

// // ================= BEGIN =================
// void LoRaWAN::begin()
// {
//     Serial.println("=== LoRaWAN Init ===");

//     // ====== Disable BLE cho RAK3172 ======
//     #if defined(rak3172)
//         // KHÔNG dùng BLE
//     #else
//         api.ble.uart.setPermission(RAK_SET_ENC_WITH_MITM);
//         api.ble.uart.start(0);
//     #endif

//     // ====== ABP CONFIG ======
//     uint8_t devAddr[4] = ABP_DEVADDR;
//     uint8_t appSKey[16] = ABP_APPSKEY;
//     uint8_t nwkSKey[16] = ABP_NWKSKEY;

//     if (!api.lorawan.njm.set(RAK_LORA_ABP)) {
//         Serial.println("Set ABP mode failed");
//         return;
//     }

//     if (!api.lorawan.daddr.set(devAddr, 4)) return;
//     if (!api.lorawan.appskey.set(appSKey, 16)) return;
//     if (!api.lorawan.nwkskey.set(nwkSKey, 16)) return;
//     if (!api.lorawan.band.set(ABP_BAND)) return;

//     if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) return;

//     // ====== LoRa PARAM ======
//     api.lorawan.adr.set(true);
//     api.lorawan.rety.set(1);
//     api.lorawan.cfm.set(0);

//     // (optional)
//     api.lorawan.dr.set(5);

//     // ====== DEBUG ======
//     uint8_t addr[4];
//     api.lorawan.daddr.get(addr, 4);
//     Serial.printf("DevAddr: %02X%02X%02X%02X\n",
//                   addr[0], addr[1], addr[2], addr[3]);

//     Serial.printf("DutyCycle: %s\n",
//                   api.lorawan.dcs.get() ? "ON" : "OFF");

//     // ====== CALLBACK ======
//     api.lorawan.registerRecvCallback(recvCallback);
//     api.lorawan.registerSendCallback(sendCallback);

//     Serial.println("=== LoRaWAN Ready ===");
// }

// void LoRaWAN::uplink_routine()
// {
//     uint8_t len = 0;

//     // Temp
//     collected_data[len++] = (g_sensor.temp >> 8) & 0xFF;
//     collected_data[len++] = g_sensor.temp & 0xFF;

//     // Humi
//     collected_data[len++] = g_sensor.humi;

//     // VBAT
//     uint16_t v = (uint16_t)(g_sensor.vbat * 1000.0 + 0.5);
//     collected_data[len++] = (v >> 8) & 0xFF;
//     collected_data[len++] = v & 0xFF;

//     // Current
//     collected_data[len++] = (current_x10 >> 8) & 0xFF;
//     collected_data[len++] = current_x10 & 0xFF;

//     // State
//     collected_data[len++] = d.state;

//     // Lux (optional – chỉ có trên RAK3172)
//     #if USE_RAK3172
//     uint16_t lux = (uint16_t)(g_sensor.lux + 0.5);
//     collected_data[len++] = (lux >> 8) & 0xFF;
//     collected_data[len++] = lux & 0xFF;
//     #endif

//     Serial.print("Payload: ");
//     for (int i = 0; i < len; i++) {
//         Serial.printf("%02X ", collected_data[i]);
//     }
//     Serial.println();

//     tx_done = false;

//     if (api.lorawan.send(len, collected_data, 2, false, 0)) {
//         Serial.println("TX Requested");
//     } else {
//         Serial.println("TX Failed");
//     }
// }

// void LoRaWAN::sleep(uint32_t sleeptime)
// {
//     Serial.printf("Sleep %us\n", sleeptime);

//     estimatedNextUplink = millis() + sleeptime * 1000;

//     api.system.sleep.all(sleeptime * 1000);   // ✅ FIX
// }

#include "LoRaWAN.h"

extern Decision d;

uint8_t minuteTimeSleep = 0;
uint32_t downlinkTimeSleep = 0;
extern bool tx_done;




void LoRaWAN::recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
  if (data->BufferSize > 0) {
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    minuteTimeSleep = data->Buffer[0];
    downlinkTimeSleep = minuteTimeSleep * 60000;
    Serial.print("Time Sleep from downlink:");
    Serial.println(downlinkTimeSleep);
    Serial.print("\r\n");
  }
}

void LoRaWAN::sendCallback(int32_t status)
{
  if (status == 0) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Sending failed");
  }
  tx_done = true;
}

void LoRaWAN::begin() {
  #if USE_RAK3172
  
  #else
    api.ble.uart.setPermission(RAK_SET_ENC_WITH_MITM);
    api.ble.uart.start(0);
    // Wake-up
    api.system.sleep.setup(RUI_WAKEUP_FALLING_EDGE, P44);
  #endif
  


    // ABP Device Address MSB first
  uint8_t node_dev_addr[4] = ABP_DEVADDR;
  // ABP Application Session Key
  uint8_t node_app_skey[16] = ABP_APPSKEY;
  // ABP Network Session Key
  uint8_t node_nwk_skey[16] = ABP_NWKSKEY;

  if (!api.lorawan.daddr.set(node_dev_addr, 4)) {
    Serial.printf("LoRaWan ABP - set device addr is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appskey.set(node_app_skey, 16)) {
    Serial.printf("LoRaWan ABP - set application session key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.nwkskey.set(node_nwk_skey, 16)) {
    Serial.printf("LoRaWan ABP - set network session key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.band.set(ABP_BAND)) {
    Serial.printf("LoRaWan ABP - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf("LoRaWan ABP - set device class is incorrect! \r\n");
    return;
  }

  #if USE_RAK3172
  if (!api.lorawan.adr.set(true)) {
        Serial.printf("LoRaWan ABP - set adaptive data rate is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.dr.set(5)) {
        Serial.printf("LoRaWan ABP - set data rate incorrect! \r\n");
        return;
    }
    if (!api.lorawan.rety.set(1)) {
        Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.cfm.set(0)) {
        Serial.printf("LoRaWan ABP - set confirm mode is incorrect! \r\n");
        return;
    }
  
    /** Check LoRaWan Status*/
    Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");	// Check Duty Cycle status
    Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");	// Check Confirm status
    uint8_t assigned_dev_addr[4] = { 0 };
    api.lorawan.daddr.get(assigned_dev_addr, 4);
    Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);	// Check Device Address
    Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
    Serial.println("");
    api.lorawan.registerRecvCallback(recvCallback);
    api.lorawan.registerSendCallback(sendCallback);
  #else
  if (!api.lorawan.njm.set(RAK_LORA_ABP)) // Set the network join mode to ABP
  {
    Serial.printf("LoRaWan ABP - set network join mode is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.adr.set(true)) {
    Serial.printf("LoRaWan ABP - set adaptive data rate is incorrect! \r\n");
    return;

  if (!api.lorawan.rety.set(1)) {
    Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(0)) {
    Serial.printf("LoRaWan ABP - set confirm mode is incorrect! \r\n");
    return;
  }
   Serial.printf("Set the join delay on RX window 1  %s\r\n", api.lorawan.jn1dl.set(5000) ? "Success" : "Fail");
  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF"); // Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
  // Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerSendCallback(sendCallback);
  }
  #endif

}

void LoRaWAN::uplink_routine()
{
  /** Payload of Uplink */
  uint8_t data_len = 0;
  collected_data[data_len++] = (uint8_t)g_sensor.temp >> 8;
  collected_data[data_len++] = (uint8_t)(g_sensor.temp / 10) & 0xFF;
  
  collected_data[data_len++] = (uint8_t)g_sensor.humi & 0xFF;

  uint16_t v_encoded = (uint16_t)(g_sensor.vbat * 1000.0 + 0.5);
  collected_data[data_len++] = (v_encoded >> 8) & 0xFF;
  collected_data[data_len++] = v_encoded & 0xFF;

  collected_data[data_len++] = (current_x10 >> 8) & 0xFF;
  collected_data[data_len++] = current_x10 & 0xFF;

  collected_data[data_len++] = (uint8_t)d.state & 0xFF;

  uint16_t lux_encoded = (uint16_t)(g_sensor.lux + 0.5);
  collected_data[data_len++] = (lux_encoded >> 8) & 0xFF;
  collected_data[data_len++] = lux_encoded & 0xFF;


  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++)
  {
    Serial.printf("0x%02X ", collected_data[i]);
  }
  Serial.println("");

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *)&collected_data, 2, false,0))
  {
    Serial.println("Sending is requested");
  }
  else
  {
    Serial.println("Sending failed");
  }
}

void LoRaWAN::sleep(uint32_t sleeptime)
{
    Serial.printf("Try sleep %us..", sleeptime);
      estimatedNextUplink = millis() + sleeptime;
      api.system.sleep.all(d.sleepTime * 1000);
}