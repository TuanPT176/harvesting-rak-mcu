#include "config.h"
#include "neh7100.h"
#include "sensor_manager.h"
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
  lora.begin();
  pmic.begin();
  pmic.writeAllConfigs();
  sensor.begin();
  
}

void loop() {

  pmic.readAll();
  current_x10 = pmic.getCurrent_uA_x10();
  pmic.ensureConfig();
  g_sensor = sensor.read();

  d = makeDecision(g_sensor.vbat, current_x10);

  if (d.sendNow) {
    lora.uplink_routine();
    delay(200); // đảm bảo TX done
    }
    lora.sleep(d.sleepTime);

  
}



// #include "Adafruit_SHTC3.h"
// #include <Wire.h>

// // Set pin number
// #define buttonPin 38
// #define SOIL_PIN 33  
// #define NEH7100_ADDR 0x3C 
// #define REG_COUNT 11       // Tổng số thanh ghi từ 0x00 đến 0x0A [cite: 1040]
// // Set Interrupt
// int ledToggle;
// int previousState = HIGH;
// unsigned int previousPress;
// volatile int buttonFlag, buttonFlag_falseDetect;
// const int buttonDebounce = 50;
// volatile int lastDetect = 0;

// volatile int rainFlag;    // turn on this flag if it is rain
// volatile int notRainFlag; // turn on this flag if it is not rain
// volatile unsigned int rainGaugeCount = 0;
// unsigned long time1 = 0;
// uint16_t estimatedNextUplink = 0;

// // Set sensor variables
// int temper;
// int humi;
// float soil_percent;

// // Rain Stop Time
// uint64_t lastRain = 0; // the last time when it was rain
// uint64_t elapsedRain;
// uint64_t spendTime; // the remaining time before wake up in period OTAA
// uint64_t donwlinkTimeSleep;
// uint8_t minuteTimeSleep;
// // Current
// uint16_t current_x10;

// bool bucketPositionA = false; // one of the two positions of tipping-bucket
// // const double bucketAmount = 0.01610595;   // inches equivalent of ml to trip tipping-bucket
// byte regValues[REG_COUNT];

// // Biến riêng cho các thanh ghi quan trọng
// byte reg_LDO_USB;     // 0x01
// byte reg_Boost;       // 0x04
// byte reg_MPPT;        // 0x05
// byte reg_IRange;      // 0x09
// byte reg_IMea;        // 0x0A

// const char* regNames[] = {
//   "LVD & OVP Settings (0x00)",
//   "LDO & USB Control (0x01)",
//   "Reserved (0x02)",
//   "Frequency Boundaries (0x03)",
//   "Boosting Factor Boundaries (0x04)",
//   "MPPT Interval (0x05)",
//   "Reserved (0x06)",
//   "Chip ID (0x07)",
//   "Status Flags (0x08)",
//   "Current Range (0x09)",
//   "Latest Current Measurement (0x0A)"
// };

// #define ABP_PERIOD   (15000)
// // #define ABP_PERIOD   (300000)
// // #define ABP_PERIOD   (900000)
// // #define ABP_PERIOD   (1800000)
// // #define RAIN_STOP_TIME   (6000)
// /*************************************

//    LoRaWAN band setting:
//      RAK_REGION_EU433
//      RAK_REGION_CN470
//      RAK_REGION_RU864
//      RAK_REGION_IN865
//      RAK_REGION_EU868
//      RAK_REGION_US915
//      RAK_REGION_AU915
//      RAK_REGION_KR920
//      RAK_REGION_AS923-1
//      RAK_REGION_AS923-2

//  *************************************/

// #define ABP_BAND     (9)
// // #define ABP_DEVADDR  {0x26, 0x0B, 0x41, 0xEB}
// // #define ABP_APPSKEY  {0x74, 0x9A, 0x23, 0x5A, 0x81, 0x0A, 0xF5, 0x37, 0x08, 0xE7, 0x22, 0x7C, 0x12, 0xA1, 0xCE, 0x5A}
// // #define ABP_NWKSKEY  {0x04, 0x58, 0x5E, 0x3E, 0x0A, 0x98, 0x4A, 0x32, 0x68, 0xEF, 0xC1, 0x8D, 0x8B, 0x22, 0x1A, 0x2F}
// //Rain Gauge 2
// #define ABP_DEVADDR  {0x26, 0x0B, 0x13, 0x3E}
// #define ABP_APPSKEY  {0x21, 0xAA, 0x2D, 0x58, 0x0B, 0xFA, 0x32, 0xD0, 0x68, 0xA9, 0xA0, 0xCE, 0x49, 0xB0, 0xAE, 0x17}
// #define ABP_NWKSKEY  {0xD6, 0xB9, 0x54, 0xC5, 0x33, 0x56, 0xB9, 0xCF, 0x96, 0xF8, 0xC5, 0x79, 0x6B, 0xCE, 0xD3, 0xED}

// // //Rain Gauge 3
// // #define ABP_DEVADDR  {0x26, 0x0B, 0xD9, 0x8E}
// // #define ABP_APPSKEY  {0x29, 0xA5, 0x90, 0xA2, 0x2C, 0x46, 0xAF, 0xA5, 0x91, 0xC2, 0x41, 0xEE, 0x9A, 0x42, 0x7C, 0x3F}
// // #define ABP_NWKSKEY  {0x4E, 0x51, 0x35, 0x26, 0x83, 0x49, 0x6B, 0xC7, 0x6E, 0xD2, 0xC1, 0xA4, 0xC2, 0x09, 0x50, 0x50}

// // //Rain Gauge 4
// // #define ABP_DEVADDR  {0x26, 0x0B, 0x80, 0x8E}
// // #define ABP_APPSKEY  {0x7D, 0xBB, 0x9F, 0xC0, 0x0C, 0xC4, 0x14, 0xCE, 0xDC, 0x5F, 0xE6, 0xFF, 0xE2, 0x8A, 0x34, 0x7A}
// // #define ABP_NWKSKEY  {0xBE, 0xF0, 0x61, 0x60, 0xFE, 0x68, 0xF2, 0xD9, 0x74, 0xE4, 0xF7, 0x47, 0xA7, 0x7C, 0x08, 0x01}


// /** Packet buffer for sending */
// uint8_t collected_data[64] = { 0 };

// Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

// void recvCallback(SERVICE_LORA_RECEIVE_T * data)
// {
//   if (data->BufferSize > 0) {
//     Serial.println("Something received!");
//     for (int i = 0; i < data->BufferSize; i++) {
//       Serial.printf("%x", data->Buffer[i]);
//     }
//     minuteTimeSleep = data->Buffer[0];
//     donwlinkTimeSleep = minuteTimeSleep*60000;
//     Serial.print("Time Sleep from downlink:");
//     Serial.println(donwlinkTimeSleep);
//     Serial.print("\r\n");
//   }
// }

// void sendCallback(int32_t status)
// {
//   if (status == 0) {
//     Serial.println("Successfully sent");
//   } else {
//     Serial.println("Sending failed");
//   }
// }

// void setup()
// {
//   Serial.begin(115200, RAK_AT_MODE);
//   delay(5000);
//   analogReadResolution(14);
//   pinMode(buttonPin, INPUT_PULLUP);
//   attachInterrupt(digitalPinToInterrupt(buttonPin), button_ISR, FALLING);

//   readAllRegister();
//   writeAllConfigs();
  
//   api.ble.uart.setPermission(RAK_SET_ENC_WITH_MITM);
//   api.ble.uart.start(0);

//   buttonFlag = 0;
//   buttonFlag_falseDetect = 0;
//   lastDetect = 0;
  
//   // Initialize SHTC3
//   Serial.println("SHTC3 test");
//   if (!shtc3.begin())
//   {
//     Serial.println("Couldn't find SHTC3");
//      //while (1) delay(1);
//   }
//   Serial.println("Found SHTC3 sensor");

//   // // Wake-up
//   // api.system.sleep.setup(RUI_WAKEUP_FALLING_EDGE, P44);

//     // ABP Device Address MSB first
//   uint8_t node_dev_addr[4] = ABP_DEVADDR;
//   // ABP Application Session Key
//   uint8_t node_app_skey[16] = ABP_APPSKEY;
//   // ABP Network Session Key
//   uint8_t node_nwk_skey[16] = ABP_NWKSKEY;

//   if (!api.lorawan.daddr.set(node_dev_addr, 4)) {
//     Serial.printf("LoRaWan ABP - set device addr is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.appskey.set(node_app_skey, 16)) {
//     Serial.printf("LoRaWan ABP - set application session key is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.nwkskey.set(node_nwk_skey, 16)) {
//     Serial.printf("LoRaWan ABP - set network session key is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.band.set(ABP_BAND)) {
//     Serial.printf("LoRaWan ABP - set band is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
//     Serial.printf("LoRaWan ABP - set device class is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.njm.set(RAK_LORA_ABP)) // Set the network join mode to ABP
//   {
//     Serial.printf("LoRaWan ABP - set network join mode is incorrect! \r\n");
//     return;
//   }

//   if (!api.lorawan.adr.set(true)) {
//     Serial.printf("LoRaWan ABP - set adaptive data rate is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.rety.set(1)) {
//     Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
//     return;
//   }
//   if (!api.lorawan.cfm.set(1)) {
//     Serial.printf("LoRaWan ABP - set confirm mode is incorrect! \r\n");
//     return;
//   }
//    Serial.printf("Set the join delay on RX window 1  %s\r\n", api.lorawan.jn1dl.set(5000) ? "Success" : "Fail");
//   /** Check LoRaWan Status*/
//   Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF"); // Check Duty Cycle status
//   Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");  // Check Confirm status
//   uint8_t assigned_dev_addr[4] = { 0 };
//   api.lorawan.daddr.get(assigned_dev_addr, 4);
//   Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);  // Check Device Address
//   Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
//   Serial.println("");
//   api.lorawan.registerRecvCallback(recvCallback);
//   api.lorawan.registerSendCallback(sendCallback);
// }

// void uplink_routine()
// {
//   /** Payload of Uplink */
//   uint8_t data_len = 0;
//   collected_data[data_len++] = (uint8_t)temper >> 8;
//   collected_data[data_len++] = (uint8_t)(temper / 10) & 0xFF;
  
//   collected_data[data_len++] = (uint8_t)humi & 0xFF;

//   uint16_t v_encoded = (uint16_t)(soil_percent * 1000.0 + 0.5);
//   collected_data[data_len++] = (v_encoded >> 8) & 0xFF;
//   collected_data[data_len++] = v_encoded & 0xFF;

//   collected_data[data_len++] = (current_x10 >> 8) & 0xFF;
//   collected_data[data_len++] = current_x10 & 0xFF;

//   Serial.println("Data Packet:");
//   for (int i = 0; i < data_len; i++)
//   {
//     Serial.printf("0x%02X ", collected_data[i]);
//   }
//   Serial.println("");

//   /** Send the data package */
//   if (api.lorawan.send(data_len, (uint8_t *)&collected_data, 2, false, 0))
//   {
//     Serial.println("Sending is requested");
//   }
//   else
//   {
//     Serial.println("Sending failed");
//   }
// }

// void loop()
// {
//   // Read SHTC3
  // sensors_event_t humidity, temp;
  // shtc3.getEvent(&humidity, &temp);
//   temper = (temp.temperature) * 10;
//   humi = humidity.relative_humidity;
//   Serial.print("Sensors values: 🌡 Temp:= ");
//   Serial.print(temper / 10);
//   Serial.println(" °C");
//   Serial.print("\t\t💧 Hum: = ");
//   Serial.print(humi);
//   Serial.println("% rH");

//   float max = 16384.0;
//   float ref = 2.4;
//   int adc_value = analogRead(SOIL_PIN);
//   delay(500);
//   adc_value = analogRead(SOIL_PIN);
//   Serial.print("adc_value: ");
//   Serial.println(adc_value);
//   float voltage = ref * ((float)adc_value / max);
//   soil_percent = voltage * 1.5;
//   Serial.print("SupCap: ");
//   Serial.print(soil_percent,3);
//   Serial.println("V");
//   writeAllConfigs();
//   delay(10);
//   readAllRegister();

//   current_x10 = getCurrent_uA_x10(reg_IRange, reg_IMea);
//   Serial.print("Current (uA x10): ");
//   Serial.println(current_x10);


//   // LoRaWAN Uplink
//   uplink_routine();
//   buttonFlag = 0;

//   // Set sleep until the next LoRaWAN Uplink
//   Serial.printf("Try sleep %ums..", ABP_PERIOD);
//   estimatedNextUplink = millis() + ABP_PERIOD;
//   api.system.sleep.all(ABP_PERIOD);

//   Serial.println("Wakeup..");
// }

// byte readRegister(byte reg) {
//   Wire.beginTransmission(NEH7100_ADDR);
//   Wire.write(reg);
//   Wire.endTransmission();

//   Wire.requestFrom(NEH7100_ADDR, 1);

//   if (Wire.available()) {
//     return Wire.read();
//   }
//   return 0xFF; // lỗi
// };

// void readAllRegister(){
//   for (byte i = 0; i < REG_COUNT; i++) {
//     // 1. Gửi địa chỉ thanh ghi muốn đọc
//     Wire.beginTransmission(NEH7100_ADDR);
//     Wire.write(i); 
//     if (Wire.endTransmission() != 0) {
//       Serial.print("Lỗi kết nối tại thanh ghi: ");
//       Serial.println(i, HEX);
//       continue;
//     }

//     // 2. Yêu cầu 1 byte dữ liệu từ thanh ghi đó
//     Wire.requestFrom(NEH7100_ADDR, 1);

//     if (Wire.available()) {
//       byte data = Wire.read();
//       regValues[i] = data;

//       // In kết quả định dạng: Tên thanh ghi | Địa chỉ | Giá trị HEX | Giá trị Binary
//       Serial.print(regNames[i]);
//       Serial.print(" [0x0"); Serial.print(i, HEX); Serial.print("]: 0x");
//       if (data < 16) Serial.print("0");
//       Serial.println(data, HEX);
//     }
//   }
//   reg_LDO_USB = regValues[0x01];
//   reg_Boost   = regValues[0x04];
//   reg_MPPT    = regValues[0x05];
//   reg_IRange  = regValues[0x09];
//   reg_IMea    = regValues[0x0A];
// };

// bool writeRegister(byte reg, byte value) {
//   Wire.beginTransmission(NEH7100_ADDR);
//   Wire.write(reg);     // Địa chỉ thanh ghi
//   Wire.write(value);   // Giá trị cần ghi

//   byte status = Wire.endTransmission();

//   if (status == 0) {
//     Serial.print("Ghi OK -> Reg 0x");
//     Serial.print(reg, HEX);
//     Serial.print(" = 0x");
//     Serial.println(value, HEX);
//     return true;
//   } else {
//     Serial.print("Ghi FAIL tại Reg 0x");
//     Serial.println(reg, HEX);
//     return false;
//   }
// }

// void writeAllConfigs() {
//   if(writeRegister(0x01, 0x67)){
//     Serial.println("WriteOK");
//   }else{
//     Serial.println("Write Fail");
//   };
//   delay(10);

//   if(writeRegister(0x04, 0x20)){
//      Serial.println("WriteOK");
//   }else{
//     Serial.println("Write Fail");
//   };
//   delay(10);

//   if(writeRegister(0x05, 0x06)){
//      Serial.println("WriteOK");
//   }else{
//     Serial.println("Write Fail");
//   };
//   delay(10);
// };

// uint16_t getCurrent_uA_x10(byte reg09, byte reg0A) {
//   byte i_range = reg09 & 0x03;
//   uint32_t current = 0;

//   switch (i_range) {
//     case 0: // 0.0706 µA
//       current = (reg0A * 706) / 1000;   // = µA ×10
//       break;
//     case 1: // 0.478 µA
//       current = (reg0A * 478) / 100;    // = µA ×10
//       break;
//     case 2: // 4.71 µA
//       current = (reg0A * 471) / 10;     // = µA ×10
//       break;
//     case 3: // 67.5 µA
//       current = reg0A * 675;            // = µA ×10
//       break;
//   }
//   return (uint16_t)current;
// }


// void button_ISR()
// {
//   int _now = millis();
//   if ((_now - lastDetect) > buttonDebounce)
//   {
//     lastDetect = _now;
//     buttonFlag++;
//   }
//   else
//   {
//     buttonFlag_falseDetect++;
//   }
// }
