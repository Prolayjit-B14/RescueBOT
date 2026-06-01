# 📄 Hardware Specifications – RescueBOT

This document outlines the detailed hardware and technical specifications of the **RescueBOT** disaster response robotic platform.

---

## 🤖 Processing Units

### 1. ESP32-WROOM-32 DevKit V1 (2x)
* **Microcontroller Core:** Dual-core 32-bit Xtensa LX6 microprocessor
* **Operating Frequency:** 240 MHz
* **SRAM / Flash:** 520 KB SRAM, 4 MB external Flash
* **Roles in System:**
  * **Telemetry Node (`tx-lora.ino`):** Gathers values from all onboard sensors (Gas, Vibration, Dual Flame, Ultrasonic, MPU6050, GPS) and broadcasts them using the SX1278 LoRa transmitter.
  * **LoRa Base Station (`rx-lora.ino`):** Receives sub-GHz radio packets at the operations station and pipes them to the control computer via USB-Serial bridge.

### 2. ESP32-CAM Module (AI-Thinker)
* **Processor / RAM:** ESP32-S dual-core Xtensa chip, with **4 MB PSRAM**
* **Camera Sensor:** OV2640 2-Megapixel CMOS sensor
* **Streaming Capability:** VGA resolution ($640 \times 480$ pixels) at ~30 FPS over Wi-Fi (Port 81 `/stream` endpoint).
* **Role in System:** **Standalone surveillance node**. It does not interface with telemetry sensors, ensuring zero SPI clock conflicts with its onboard PSRAM.

### 3. Arduino Nano V3 (ATmega328P) (2x)
* **Processor Core:** 8-bit AVR RISC microcontroller
* **Clock Speed:** 16 MHz
* **Roles in System:**
  * **Transmitter Remote (`tx.ino`):** Polls the dual XY Joysticks and Grip Switch on the handheld controller and transmits commands via nRF24L01+.
  * **Receiver Chassis (`rx.ino`):** Receives remote commands and handles drive motor L298N direction/PWM speed signals and 4DOF robotic arm servos.

---

## 📡 Wireless Communication Systems

### 1. nRF24L01+ PA+LNA (Control Link)
* **Frequency Band:** 2.4 GHz ISM (Channel 108)
* **Data Transfer Rate:** 250 kbps (optimized for maximum sensitivity and range)
* **Role:** Transmits smoothed joystick driving and arm manipulation packets from controller to chassis.

### 2. LoRa SX1278 (RA-02 Module) (Telemetry Link)
* **Frequency Band:** 433 MHz Carrier
* **Protocol Interface:** SPI (Serial Peripheral Interface)
* **Role:** Broadcasts environment sensor data packets from the rover to the base station.

---

## 🧪 Sensors Array
All sensors are wired to the **Telemetry ESP32** on the rover:

| Sensor Module | Model Number | Interface | Electrical Specs | Operational Parameters |
| :--- | :--- | :--- | :--- | :--- |
| **Gas & Smoke** | MQ-2 Module | Analog ADC | 5.0V VCC, < 150mA | Detects LPG, Propane, Hydrogen, and smoke |
| **Vibration** | SW-420 Sensor | Analog ADC | 3.3V - 5V VCC | Measures impact/structural shifts |
| **Flame Detector** | KY-026 (2x) | Digital GPIO | 3.3V VCC, active-low | Dual IR phototransistor array ($760\text{ nm} - 1100\text{ nm}$ detection angle) |
| **Tilt & Gyro** | GY-521 MPU6050 | I2C (0x68) | 3.3V VCC | 3-axis accelerometer and gyro, calculates Pitch & Roll orientation |
| **GPS Tracker** | u-blox NEO-6M | UART Serial | 3.3V - 5V VCC | Location tracking via UART output at 9600 baud |
| **Ultrasonic** | HC-SR04 | Digital GPIO | 5.0V VCC | Distance range $2\text{ cm} - 400\text{ cm}$ (for forward collision avoidance) |

---

## ⚙ Motion & Mechanical System
* **Motor Controller:** L298N Dual H-Bridge driver
* **Drive Motors:** 4x Yellow DC Geared BO Motors (driven at battery rail voltage)
* **Robotic Arm Actuators:** 4DOF articulated design
  * **Shoulder & Elbow:** 2x MG90S Servos (metal gears, $2.2\text{ kg}\cdot\text{cm}$ torque at 6V)
  * **Claw & Pan-Sweep:** 2x SG90 Servos (plastic gears, $1.6\text{ kg}\cdot\text{cm}$ torque at 4.8V)
* **Chassis Frame:** Acrylic multi-wheel baseplate.

---

## 🔋 Power Management Subsystem
* **Main Batteries:** 2S Li-ion battery pack (18650 cells, nominal $7.4\text{V}$, peak $8.4\text{V}$)
* **Charger Interface:** TP4056 Linear battery charging module
* **Power Regulation Rails:** 5.0V switching buck regulator for motors/servos/buzzer, and 3.3V LDO for logic controllers/sensors.
