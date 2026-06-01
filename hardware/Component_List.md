# 📄 Component List – RescueBOT

## 🛠 Hardware Components Used

This document lists all components used in **RescueBOT**, an intelligent disaster response and rescue robotic system designed for hazardous and emergency environments.

---

# 🔹 Core Controllers

| Sl. No | Component               | Model Name / Part Number     | Qty | Purpose                                                                                                     |
| ------ | ----------------------- | ---------------------------- | --- | ----------------------------------------------------------------------------------------------------------- |
| 1      | ESP32 Development Board | ESP32-WROOM-32 DevKit V1     | 2   | One ESP32 collects sensor data in rover, another acts as base station for LoRa communication and web server |
| 2      | ESP32-CAM Module        | AI-Thinker ESP32-CAM         | 1   | Live video monitoring and camera streaming                                                                  |
| 3      | Arduino Nano            | Arduino Nano V3 (ATmega328P) | 2   | TX/RX rover control system — one for robot movement and one for robotic arm control                         |

---

# 🔹 Sensors

| Sl. No | Component                 | Model Name / Part Number   | Qty | Purpose                                     |
| ------ | ------------------------- | -------------------------- | --- | ------------------------------------------- |
| 4      | Flame Sensor              | KY-026 Flame Sensor Module | 1   | Detects fire presence                       |
| 5      | Gas & Smoke Sensor        | MQ-2 Gas Sensor Module     | 1   | Detects smoke and hazardous gases           |
| 6      | Ultrasonic Sensor         | HC-SR04 Ultrasonic Module  | 1   | Obstacle detection and distance measurement |
| 7      | Gyroscope & Accelerometer | MPU6050 (GY-521)           | 1   | Motion, tilt and orientation sensing        |
| 8      | GPS Module                | u-blox NEO-6M GPS Module   | 1   | Location tracking                           |
| 9      | Vibration Sensor          | SW-420 Vibration Sensor    | 1   | Detects vibration/collapse situations       |

---

# 🔹 Communication Modules

| Sl. No | Component   | Model Name / Part Number          | Qty | Purpose                                                               |
| ------ | ----------- | --------------------------------- | --- | --------------------------------------------------------------------- |
| 10     | nRF Module  | nRF24L01+ PA+LNA with SMA Antenna | 2   | Short-range wireless TX/RX communication between controller and rover |
| 11     | LoRa Module | SX1278 LoRa RA-02 (433 MHz)       | 2   | Long-range communication between rover ESP32 and base station ESP32   |

---

# 🔹 Motion & Actuation

| Sl. No | Component            | Model Name / Part Number         | Qty | Purpose                                      |
| ------ | -------------------- | -------------------------------- | --- | -------------------------------------------- |
| 12     | Motor Driver         | L298N Dual H-Bridge Motor Driver | 1   | Controls rover DC motors                     |
| 13     | DC Gear Motors       | BO Motor / Geared DC Motor       | 4   | Robot movement                               |
| 14     | Servo Motors (Blue)  | SG90 Micro Servo (Blue)          | 2   | Robotic arm movement                         |
| 15     | Servo Motors (Black) | MG90S Metal Gear Servo (Black)   | 2   | Robotic arm movement                         |
| 16     | Robotic Gripper      | 2-Finger Robotic Gripper         | 1   | Rescue object gripping                       |
| 17     | Robotic Arm Assembly | Servo Based 4DOF Arm             | 1   | Manipulation of objects in rescue operations |
| 18     | Robot Chassis        | Smart Robot Car Chassis          | 1   | Structural body/frame of robot               |

---

# 🔹 User Control Components

| Sl. No | Component       | Model Name / Part Number        | Qty      | Purpose                                                         |
| ------ | --------------- | ------------------------------- | -------- | --------------------------------------------------------------- |
| 19     | Joystick Module | PS2 XY Joystick Module (KY-023) | 2        | One joystick for rover movement and one for robotic arm control |
| 20     | Push Buttons    | Tactile Push Button Switch      | Multiple | Manual trigger/control functions                                |
| 21     | Toggle Switch   | SPST/SPDT Switch                | Multiple | Power and control switching                                     |

---

# 🔹 Display & Indicators

| Sl. No | Component      | Model Name / Part Number  | Qty      | Purpose                            |
| ------ | -------------- | ------------------------- | -------- | ---------------------------------- |
| 22     | OLED Display   | SSD1306 0.96" OLED I2C    | 1        | Displays sensor/system information |
| 23     | Buzzer         | Active Piezo Buzzer       | 1        | Alarm and warning indication       |
| 24     | LED Indicators | 5mm LEDs (Red/Green/Blue) | Multiple | Status and warning indication      |

---

# 🔹 Power Management

| Sl. No | Component            | Model Name / Part Number       | Qty | Purpose                         |
| ------ | -------------------- | ------------------------------ | --- | ------------------------------- |
| 25     | Rechargeable Battery | 18650 Li-ion Battery Pack      | 1   | Main power source               |
| 26     | Charging Module      | TP4056 Battery Charging Module | 1   | Battery charging and protection |

---

# 🔹 Prototyping & Wiring

| Sl. No | Component              | Model Name / Part Number   | Qty      | Purpose                                  |
| ------ | ---------------------- | -------------------------- | -------- | ---------------------------------------- |
| 27     | Vero Board / Perfboard | General Purpose Perfboard  | 1        | Circuit soldering and permanent assembly |
| 28     | Breadboard             | MB-102 Breadboard          | 1        | Prototype testing                        |
| 29     | Jumper Wires           | Dupont Male/Female Wires   | Multiple | Temporary electrical connections         |
| 30     | Hookup Wire            | 22 AWG Wire                | Multiple | Permanent circuit wiring                 |
| 31     | Connectors             | Berg Strip / JST Connector | Multiple | Electrical interconnection               |

---

# 🔹 Tools & Equipment Used

| Sl. No | Tool             | Model / Type              | Purpose               |
| ------ | ---------------- | ------------------------- | --------------------- |
| 32     | Soldering Iron   | 25W/60W Soldering Iron    | Circuit soldering     |
| 33     | Solder Wire      | Rosin Core Solder Wire    | Electrical joining    |
| 34     | Desoldering Tool | Desolder Pump / Wick      | Removing solder       |
| 35     | Multimeter       | DT830D Digital Multimeter | Debugging and testing |
| 36     | Wire Cutter      | Mini Wire Cutter          | Wire preparation      |
| 37     | Screwdriver Set  | Precision Screwdriver Kit | Assembly              |

---

## 📡 System Architecture Summary

* **Arduino Nano 1 (TX/RX):** Rover movement control using joystick and nRF communication
* **Arduino Nano 2 (TX/RX):** Robotic arm control using joystick and nRF communication
* **ESP32 Rover Node:** Collects sensor data and sends telemetry through LoRa
* **ESP32 Base Station:** Receives LoRa data and hosts web dashboard/server
* **ESP32-CAM:** Live monitoring and surveillance feed

---

**Project Name:** RescueBOT
**Event:** ZYRO 2026 Hackathon – KGEC
**Team:** BOT THINGS
