# 📄 Hardware Specifications – RescueBOT

## 🔧 System Overview

RescueBOT is a disaster response robotic platform designed for rescue, monitoring, and hazardous environment navigation.

---

## 🤖 Processing Units

### ESP32-WROOM-32 DevKit V1 (2x)

* Dual-core Xtensa LX6 Processor
* Clock Speed: 240 MHz
* Wi-Fi: 802.11 b/g/n
* Bluetooth: BLE + Classic
* Operating Voltage: 3.3V
* Purpose:

  * Rover sensor data acquisition
  * Base station communication and web server

### ESP32-CAM (AI Thinker)

* OV2640 Camera Module
* Wi-Fi + Bluetooth
* MicroSD support
* Purpose:

  * Live monitoring
  * Visual inspection

### Arduino Nano V3 (ATmega328P) (2x)

* Operating Voltage: 5V
* Clock Speed: 16 MHz
* Digital I/O Pins: 14
* Analog Inputs: 8
* Purpose:

  * Rover movement control
  * Robotic arm control

---

## 📡 Communication System

### nRF24L01+ PA+LNA

* Frequency: 2.4 GHz
* Long-range external antenna
* SPI communication
* Purpose:

  * Controller ↔ Rover communication

### LoRa SX1278 (433 MHz)

* Long-range low-power communication
* Range: up to several kilometers (open area)
* Purpose:

  * Sensor telemetry transmission

---

## 🧪 Sensors

| Sensor       | Model   | Function                       |
| ------------ | ------- | ------------------------------ |
| Flame Sensor | KY-026  | Fire detection                 |
| Gas Sensor   | MQ-2    | Smoke and gas detection        |
| Ultrasonic   | HC-SR04 | Distance sensing               |
| MPU6050      | GY-521  | Motion sensing                 |
| GPS          | NEO-6M  | Position tracking              |
| Vibration    | SW-420  | Structural vibration detection |

---

## ⚙ Motion System

| Component    | Specification                 |
| ------------ | ----------------------------- |
| Motor Driver | L298N Dual H-Bridge           |
| Drive Motors | 4× DC Geared Motors           |
| Servo Motors | 2× Blue SG90 + 2× Black MG90S |
| Gripper      | Two-finger robotic gripper    |
| Chassis      | Multi-wheel robotic chassis   |

---

## 🔋 Power System

* Battery Type: Rechargeable Li-ion (18650)
* Charging Module: TP4056
* Operating Voltage: 3.3V / 5V

---

## 🖥 Display & Indicators

* OLED Display: SSD1306 0.96" I2C
* Audio Alert: Active buzzer
* Status Indicators: LEDs

---

## 🎮 Control Interface

* 2× PS2 XY Joystick Modules
* Toggle switches
* Push buttons

---

## 📌 System Architecture

1. Sensors → ESP32 Rover Node
2. ESP32 Rover → LoRa Transmission
3. LoRa → Base Station ESP32
4. Base Station → Web Server Dashboard
5. Arduino Nano → Motor + Arm Control via nRF

---

**Project:** RescueBOT
**Event:** ZYRO 2026 Hackathon – KGEC
**Team:** BOT THINGS
