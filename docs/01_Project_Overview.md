# 📄 01 — Project Overview: RescueBOT

> **Autonomous Disaster Response & Rescue Robotic Platform**
> *Developed by Team BOT THINGS | ZYRO 2026 Hackathon | Kalyani Government Engineering College (KGEC)*

---

## 1. Executive Summary

**RescueBOT** is a rugged, multi-modal autonomous disaster response and rescue robotic platform (Unmanned Ground Vehicle — UGV) engineered to navigate structurally compromised, chemically hazardous, and GPS-degraded environments where human rescuers cannot safely operate. Designed as a force-multiplier for emergency response teams, the system integrates a high-fidelity multi-sensor telemetry array, dual-band wireless communications, live video surveillance, and a physically interactive 4DOF robotic gripper arm — all packaged into a low-cost prototype that costs under ₹10,000 (≈ USD 120) in bill-of-materials.

The platform is built around a **distributed microcontroller architecture**: an ESP32 DevKit handles environmental telemetry and long-range LoRa transmission, an Arduino Nano pair manages RF control and motor/servo actuation, and a standalone ESP32-CAM module provides independent live visual feeds. These subsystems communicate over separated frequency bands (2.4 GHz control, 433 MHz telemetry) to eliminate RF cross-interference in obstacle-dense disaster zones.

RescueBOT was prototyped, tested, and demonstrated at the **ZYRO 2026 Hackathon** at KGEC under the Open Innovation track, where it proved reliable sensor detection, real-time telemetry transmission through three concrete walls, and precise joystick-driven manipulation of the robotic arm.

---

## 2. Core Problem Domain & Real-World Relevance

### The Cost of Delayed Rescue

According to the United Nations Office for Disaster Risk Reduction (UNDRR), between 2000 and 2020 over 1.23 million people lost their lives in natural disasters globally. Earthquake response statistics compiled by FEMA and international rescue agencies consistently show that **survival probability drops below 5% after 72 hours** for victims trapped in collapsed structures. The primary bottlenecks are:

- **Rescuer Danger:** Responders entering unstable rubble fields, burning buildings, or gas-filled rooms risk their own lives before locating a single survivor.
- **Environmental Blindness:** Incident commanders make decisions without real-time gas concentration, temperature gradient, or structural vibration data from inside the hazard zone.
- **Communication Infrastructure Loss:** Cellular towers and Wi-Fi access points are among the first systems to fail in major disasters, cutting rescue teams off from coordinated intelligence.
- **Speed of Deployment:** Specialized industrial rescue robots (e.g., GRYPHON, CHIMP, PROBOT) cost upwards of $12,000–$400,000 USD and require trained operators, making them inaccessible to municipal or rural rescue teams.

RescueBOT directly addresses each of these failure modes with a deployable, sub-$150 robotic scout.

---

## 3. Major System Modules

RescueBOT is built from five tightly integrated functional modules:

```
┌──────────────────────────────────────────────────────────────────────────┐
│                         RESCUEBOT SYSTEM MODULES                         │
├─────────────────────┬──────────────────────────────┬─────────────────────┤
│  MODULE             │  HARDWARE                    │  FUNCTION           │
├─────────────────────┼──────────────────────────────┼─────────────────────┤
│ 1. Telemetry Node   │ ESP32 + SX1278 LoRa          │ Sensor fusion &     │
│                     │ MQ-2, SW-420, KY-026 (×2)    │ data transmission   │
│                     │ MPU6050, NEO-6M, HC-SR04      │                     │
├─────────────────────┼──────────────────────────────┼─────────────────────┤
│ 2. Actuation Node   │ Arduino Nano RX              │ Drive control &     │
│                     │ L298N + 4× BO Motors         │ robotic arm         │
│                     │ MG90S + SG90 Servos          │ manipulation        │
├─────────────────────┼──────────────────────────────┼─────────────────────┤
│ 3. Remote Control   │ Arduino Nano TX              │ Joystick-to-RF      │
│                     │ 2× Joysticks, nRF24L01+      │ command encoding    │
├─────────────────────┼──────────────────────────────┼─────────────────────┤
│ 4. Surveillance     │ ESP32-CAM (OV2640)           │ Live MJPEG video    │
│    Node             │ Flash LED (GPIO 4 PWM)        │ stream on Port 81   │
├─────────────────────┼──────────────────────────────┼─────────────────────┤
│ 5. Base Station     │ ESP32 LoRa RX                │ Telemetry relay to  │
│    Receiver         │ SX1278 + USB Serial Bridge   │ web dashboard       │
└─────────────────────┴──────────────────────────────┴─────────────────────┘
```

---

## 4. High-Level System Architecture

```
[OPERATOR REMOTE CONTROLLER]           [RESCUEBOT ROVER CHASSIS]
┌──────────────────────────┐           ┌───────────────────────────────────┐
│  Arduino Nano TX         │           │  Arduino Nano RX                  │
│  ├─ Dual Joysticks       │           │  ├─ L298N ──► 4× BO Motors        │
│  └─ Grip Toggle Switch   │           │  └─ 4× Servos (Arm + Sweep)       │
│  nRF24L01+ PA+LNA ───────┼──2.4 GHz──┼► nRF24L01+ PA+LNA                │
└──────────────────────────┘           │                                   │
                                        │  ESP32 Telemetry Node             │
[BASE OPERATIONS STATION]              │  ├─ MQ-2, SW-420, KY-026 ×2       │
┌──────────────────────────┐           │  ├─ MPU6050, NEO-6M, HC-SR04      │
│  Web Dashboard + PC      │           │  ├─ Buzzer, Red/Green LEDs        │
│  ESP32 LoRa Receiver ◄───┼──433 MHz──┼─ SX1278 LoRa TX                  │
└──────────────────────────┘           │                                   │
                                        │  ESP32-CAM Node                   │
                                        │  └─ OV2640 ──► Wi-Fi Port 81 ────┼──► Dashboard
                                        └───────────────────────────────────┘
```

---

## 5. Key Technical Specifications

| Parameter | Value |
|:---|:---|
| **Platform Type** | 4-Wheel Differential Drive UGV |
| **Chassis** | Multi-layer acrylic car chassis |
| **Primary MCU** | ESP32-WROOM-32 (240 MHz dual-core Xtensa LX6) |
| **Secondary MCU** | Arduino Nano V3.0 (16 MHz ATmega328P) ×2 |
| **Control Range (nRF)** | Up to ~200 m line-of-sight at 250 kbps |
| **Telemetry Range (LoRa)** | 350 m urban / 1.2 km line-of-sight |
| **Video Resolution** | VGA 640×480 at ~22 FPS (with PSRAM) |
| **Sensor Polling Rate** | ~1 Hz (1000 ms cycle) |
| **Control Latency** | ~35 ms (joystick-to-motor response) |
| **Fail-Safe Timeout** | 500 ms (auto-halt on signal loss) |
| **Power Supply** | 2S Li-ion 18650 (7.4V–8.4V) |
| **Bill of Materials** | ≤ ₹9,970 (≈ USD 120) |
| **Operating Environment** | Earthquakes, fires, floods, gas leaks |

---

## 6. Real-World Application Use Cases

### 6.1 Post-Earthquake Structural Reconnaissance
RescueBOT can be deployed into collapsed masonry structures where aftershock risk prevents human entry. The SW-420 seismic sensor continuously monitors vibration intensity; the MPU6050 IMU detects tilt angles >15° that indicate structural instability; and the NEO-6M GPS tracks the UGV's last known surface coordinates. The operator monitors all readings from the base station dashboard in real time.

### 6.2 Industrial Gas Leak & Fire Response
In chemical plant or refinery incidents, RescueBOT's MQ-2 sensor detects combustible gases (LPG, CH₄, H₂, CO) and its dual KY-026 IR flame sensors (sensitive to 760–1100 nm wavelengths) detect active fire sources. The local alarm (Red LED + 2500 Hz buzzer) triggers immediately when thresholds are breached, providing both local and remote alerts.

### 6.3 Emergency Cargo Delivery
The 4DOF robotic arm (Shoulder, Elbow, Gripper Claw, Sweep Pan) allows operators to pick up and deposit small payloads (medicines, communication devices, water pouches) from 50–150 meter distances using the nRF control link. The gripper operates between 0° (fully open) and 90° (fully closed), driven by MG90S metal-gear servos capable of sustained operation under load.

### 6.4 Mine, Tunnel & Indoor Disasters
The 433 MHz LoRa link can propagate through multiple concrete walls (tested reliably through 3 concrete walls at RSSI −98 dBm), making RescueBOT operable in GPS-denied underground environments where standard Wi-Fi or Bluetooth systems fail completely.

---

## 7. Hackathon Presentation & Impact Statement

RescueBOT was presented at **ZYRO 2026** at Kalyani Government Engineering College under the *Open Innovation* track. The live demonstration included:
- Real-time joystick-driven navigation on a simulated rubble course.
- Triggered gas and flame hazard alerts with LoRa telemetry relayed to the base station dashboard.
- Live MJPEG video stream from the ESP32-CAM module.
- Robotic arm operation to pick and place a simulated medical payload.

The project represents a proof-of-concept for **affordable, modular rescue robotics** scalable to multi-unit swarm deployments for large-scale disaster response operations.

---

*Next: [02 — Problem Statement →](./02_Problem_Statement.md)*
