#include "LoRaWAN.h"

extern Decision d;

uint8_t minuteTimeSleep = 0;
uint32_t downlinkTimeSleep = 0;


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
}

void LoRaWAN::begin() {
  api.ble.uart.setPermission(RAK_SET_ENC_WITH_MITM);
  api.ble.uart.start(0);
    // Wake-up
  api.system.sleep.setup(RUI_WAKEUP_FALLING_EDGE, P44);

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
  if (!api.lorawan.njm.set(RAK_LORA_ABP)) // Set the network join mode to ABP
  {
    Serial.printf("LoRaWan ABP - set network join mode is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.adr.set(true)) {
    Serial.printf("LoRaWan ABP - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(1)) {
    Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(1)) {
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
  Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerSendCallback(sendCallback);
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

  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++)
  {
    Serial.printf("0x%02X ", collected_data[i]);
  }
  Serial.println("");

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *)&collected_data, 2, false, 0))
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
    Serial.printf("Try sleep %ums..", sleeptime);
      estimatedNextUplink = millis() + sleeptime;
      api.system.sleep.all(d.sleepTime * 1000);
}