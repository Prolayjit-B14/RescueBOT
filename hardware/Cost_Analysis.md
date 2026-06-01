# 📄 Cost Analysis – RescueBOT

This document provides a comprehensive cost breakdown and analysis of the components used in the **RescueBOT** project. It groups components by functional sub-systems, compares alternative communication setups, and suggests cost-optimization methods.

---

## 💰 Component Cost Breakdown

The table below details the components required for the RescueBOT system, with corrected quantities matching the active firmware (e.g., dual IR flame sensors).

> **Note:** Prices listed are approximations in Indian Rupees (INR) and may vary depending on local suppliers and bulk purchasing.

### 1. Control & Processing Units
| Sl. No | Component | Model / Specs | Qty | Unit Cost (INR) | Total Cost (INR) |
| :--- | :--- | :--- | :---: | :---: | :---: |
| 1.1 | ESP32 Dev Board | ESP32-WROOM-32 DevKit V1 | 2 | ₹350 | ₹700 |
| 1.2 | ESP32-CAM Module | AI-Thinker (with OV2640) | 1 | ₹650 | ₹650 |
| 1.3 | Arduino Nano | V3.0 (ATmega328P) | 2 | ₹300 | ₹600 |
| **Subtotal** | | | | | **₹1,950** |

### 2. Sensor Array
| Sl. No | Component | Model / Specs | Qty | Unit Cost (INR) | Total Cost (INR) |
| :--- | :--- | :--- | :---: | :---: | :---: |
| 2.1 | IR Flame Sensor | KY-026 (Double array in code) | 2 | ₹80 | ₹160 |
| 2.2 | Gas & Smoke Sensor | MQ-2 Sensor Module | 1 | ₹150 | ₹150 |
| 2.3 | Ultrasonic Sensor | HC-SR04 Module | 1 | ₹100 | ₹100 |
| 2.4 | Gyroscope & Accel | MPU6050 (GY-521) | 1 | ₹120 | ₹120 |
| 2.5 | GPS Module | u-blox NEO-6M | 1 | ₹650 | ₹650 |
| 2.6 | Vibration Sensor | SW-420 Seismic Sensor | 1 | ₹80 | ₹80 |
| **Subtotal** | | | | | **₹1,260** |

### 3. Wireless Communication Modules
| Sl. No | Component | Model / Specs | Qty | Unit Cost (INR) | Total Cost (INR) |
| :--- | :--- | :--- | :---: | :---: | :---: |
| 3.1 | nRF24L01+ Transceiver | PA+LNA (with SMA Antenna) | 2 | ₹250 | ₹500 |
| 3.2 | LoRa Module | SX1278 RA-02 (433 MHz) | 2 | ₹450 | ₹900 |
| **Subtotal** | | | | | **₹1,400** |

### 4. Motion & Actuation Subsystem
| Sl. No | Component | Model / Specs | Qty | Unit Cost (INR) | Total Cost (INR) |
| :--- | :--- | :--- | :---: | :---: | :---: |
| 4.1 | Motor Driver | L298N Dual H-Bridge | 1 | ₹180 | ₹180 |
| 4.2 | DC Geared Motors | BO Gear Motors | 4 | ₹150 | ₹600 |
| 4.3 | Micro Servos (Blue) | SG90 (Robotic Arm/Plaform) | 2 | ₹180 | ₹360 |
| 4.4 | Metal Gear Servos | MG90S (Robotic Arm/Plaform) | 2 | ₹180 | ₹360 |
| 4.5 | Robotic Gripper | 2-Finger Acrylic Claw | 1 | ₹350 | ₹350 |
| 4.6 | Robot Chassis | Multi-Wheel Acrylic Plate | 1 | ₹900 | ₹900 |
| **Subtotal** | | | | | **₹2,750** |

### 5. Display, Indicators, & Control Interface
| Sl. No | Component | Model / Specs | Qty | Unit Cost (INR) | Total Cost (INR) |
| :--- | :--- | :--- | :---: | :---: | :---: |
| 5.1 | Joystick Module | PS2 XY Joystick | 2 | ₹100 | ₹200 |
| 5.2 | OLED Display | SSD1306 0.96" I2C | 1 | ₹250 | ₹250 |
| 5.3 | Active Piezo Buzzer | 5V Buzzer | 1 | ₹30 | ₹30 |
| 5.4 | LEDs (R/G/B) | 5mm indicators | Multiple | ₹50 | ₹50 |
| **Subtotal** | | | | | **₹530** |

### 6. Power, Prototyping, & Accessories
| Sl. No | Component | Model / Specs | Qty | Unit Cost (INR) | Total Cost (INR) |
| :--- | :--- | :--- | :---: | :---: | :---: |
| 6.1 | Li-ion Battery Pack | 18650 7.4V Pack | 1 | ₹400 | ₹400 |
| 6.2 | Charging Module | TP4056 Charger | 1 | ₹80 | ₹80 |
| 6.3 | Prototyping Boards | Vero board (1x) & Breadboard (1x)| 2 | ₹135 | ₹270 |
| 6.4 | Wiring & Connectors | Jumper / Hookup Wires / Berg strips| - | ₹320 | ₹320 |
| 6.5 | Assembly Tools | Solder wire, switches, screws | - | ₹1,000 | ₹1,000 |
| **Subtotal** | | | | | **₹2,070** |

---

## 💵 Project Cost Summary

* **Net Bill of Materials (BOM):** **₹9,970**
* **Projected Total Budget (with tools/spares buffer):** **₹10,000 – ₹12,000 (Approx.)**

---

## 📡 Wireless Communication Cost-Benefit Comparison

The RescueBOT firmware supports two distinct communication topologies. Depending on the deployment environment, one network path can be prioritized to save cost:

| Topology Route | Components Required | Approx Cost | Range | Pros | Cons |
| :--- | :--- | :---: | :--- | :--- | :--- |
| **Route A: Wi-Fi & MQTT** | ESP32-CAM, ESP32 Sensor Node, Local AP / Hotspot | **₹1,350** | ~50 meters | * Transmits live video feeds (VGA/QVGA).<br>* Direct cloud logging to EMQX Broker.<br>* Beautiful web UI updating in real-time. | * High latency in crowded Wi-Fi space.<br>* Range limited to Wi-Fi AP footprint. |
| **Route B: LoRa Telemetry** | 2x ESP32 boards, 2x SX1278 LoRa transceivers | **₹1,600** | Up to several kilometers | * Highly robust against obstacles (sewers, rubble).<br>* Dedicated sub-GHz frequency link. | * Low bandwidth (No video streaming).<br>* Comma-separated string parsing required. |

---

## 📌 Cost Optimization Strategies

1. **Selective Component Population:**
   If deploying in local offline range, the LoRa transceivers (saving **₹900**) or the Wi-Fi client modules can be selectively depopulated.
2. **Standardize Servos:**
   Using micro-servo SG90 models (approx. ₹120 each) instead of MG90S metal-gear servos for low-stress joints (e.g. Sweep panning servo) will trim the mechanical budget.
3. **Power Distribution Consolidation:**
   Replace individual battery charging modules with a unified 2S battery pack board containing built-in BMS (Battery Management System) and a 5V buck regulator to avoid burning out Arduinos and ESPs with high voltage.
4. **Use Board Clones:**
   Utilizing clone boards for Arduino Nano (using CH340 USB drivers) instead of original FTDI versions drops unit controller cost from ~₹300 to ~₹180.

---

**Project Name:** RescueBOT  
**Event:** ZYRO 2026 Hackathon – KGEC  
**Team:** BOT THINGS  
