# 🌧️ Rain Gauge LoRaWAN Node (RAK3172 / RAK11720)

This project is an **energy-harvesting LoRaWAN sensor node** for environmental monitoring. It supports ultra-low power operation using a supercapacitor and solar harvesting.

---

# 🔗 Live Dashboard (Grafana)

📊 You can monitor real-time data here:
👉 **[Grafana Dashboard](https://your-grafana-link-here.com)**

Features:

* Battery voltage (supercapacitor)
* Harvesting current
* Temperature & humidity
* Light intensity (lux)
* Node state (energy mode)

---

# 📂 Project Structure

```id="h4b3f0"
.
├── src/              # Firmware source code
├── docs/             # Documentation
├── Hardware/         # Open-source hardware (PCB, schematic)
└── main.ino          # Main firmware
```

---

# ⚙️ System Overview

## 🧩 System Architecture

![System Diagram](docs/system_diagram.png)

> 📌 Replace this image with your actual diagram inside `/docs`

---

## 🔋 Energy Flow

```id="0t9s6o"
Solar Panel → NEH7100 → Supercapacitor → MCU (RAK3172)
```

* NEH7100 handles MPPT and regulation
* Supercapacitor stores energy (max 3.8V)
* MCU adapts behavior based on energy availability

---

# ⚙️ Firmware Features

* Adaptive scheduling (energy-aware)
* LoRaWAN uplink (TTN compatible)
* Multi-sensor support:

  * SHTC3 (Temp/Humidity)
  * LTR303 (Lux - optional)
* Dynamic sleep control

---

# 🚀 Getting Started

## 1. Hardware Setup

* RAK3172 / RAK11720
* NEH7100 PMIC
* Supercapacitor (≤3.8V)
* Sensors (SHTC3, optional LTR303)

---

## 2. Software Setup

* Install Arduino IDE
* Install RAKwireless BSP in Arduino IDE
* Select correct board

---

## 3. Configuration

Edit `config.h` and `sensor_manager.h`:

```cpp id="i2z2p3"
#define USE_RAK3172 1
#define VBAT_PIN PA15
```

---

## 4. LoRaWAN (TTN)

Supports:

* OTAA 
* ABP (Used)

---

# 📦 Payload Format

| Byte | Data                       |
| ---- | ---------------------------|
| 0-1  | Temperature                |
| 2    | Humidity                   |
| 3-4  | Voltage supercapacitor     |
| 5-6  | Current                    |
| 7    | State                      |
| 8-9  | Lux                        |
---

# 🧠 Energy-Aware Scheduler

| State | Description                  |
| ----- | ---------------------------- |
| 30    | Critical (sleep long, no TX) |
| 20/21 | Low energy                   |
| 10/11 | Normal                       |
| 0     | High energy                  |

---

# 🛠️ Hardware (Open Source)

📁 Located in `/Hardware`

## Included Files:

* 📐 Schematic (PDF)
* 🧱 PCB Layout (Altium/KiCad)
* 📋 BOM (Bill of Materials)

## Design Highlights:

* NEH7100 energy harvesting PMIC
* Supercapacitor storage
* Low-leakage voltage divider
* Optional LTR303 light sensor

---

# 📊 Hardware Notes

## VBAT Measurement

```id="1h4e3g"
VBAT → Divider → ADC (PA15 with RAK3172 and 33 with RAK11720)
```

---

# 📈 Calibration

## Voltage:

```cpp id="d0c1gh"
V = Vref * (ADC / 4095.0) * scale_factor;
```

## Lux:

```cpp id="drh6j3"
lux = 0.07 * CH0 - 0.02 * CH1;
```

---

# 🔬 Future Work

* Adaptive MPPT tuning
* OTA firmware update
* Multi-node synchronization
* AI-based energy prediction

---

# 🤝 Contributing

PRs and issues are welcome!

---


# 👨‍🔬 Author

Developed for:

* Energy harvesting IoT
* LoRaWAN research
* Low-power embedded systems

---

🚀 *Autonomous IoT powered by ambient energy*
