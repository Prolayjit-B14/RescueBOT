<div align="center">

<img src="media/logo_RescueBOT.png" alt="RescueBOT Logo" width="180"/>

# RescueBOT
### Autonomous Disaster Response & Rescue Robotic Platform

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue.svg)](https://www.espressif.com/)
[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-teal.svg)](https://www.arduino.cc/)
[![RF: LoRa 433MHz](https://img.shields.io/badge/RF-LoRa%20433MHz-orange.svg)](#)
[![Hackathon: ZYRO 2026](https://img.shields.io/badge/Hackathon-ZYRO%202026%20%40%20KGEC-purple.svg)](#)
[![Team: BOT THINGS](https://img.shields.io/badge/Team-BOT%20THINGS-red.svg)](#)
[![Dashboard Repo](https://img.shields.io/badge/Dashboard-Diaster__Rover__Dashboard-success.svg)](https://github.com/Prolayjit-B14/Diaster_Rover_Dashboard)

**Built by Team BOT THINGS | ZYRO 2026 Hackathon | Kalyani Government Engineering College**

[📄 Docs](#-documentation) · [🔧 Hardware Setup](HARDWARE_SETUP.md) · [💻 Software Setup](SOFTWARE_SETUP.md) · [🗺️ Roadmap](ROADMAP.md) · [📋 Changelog](CHANGELOG.md) · [🤝 Contributing](CONTRIBUTING.md)

</div>

---

## 🤖 What is RescueBOT?

**RescueBOT** is a low-cost, open-source autonomous disaster response UGV (Unmanned Ground Vehicle) designed to enter hazardous environments where human rescuers cannot safely operate. It fuses real-time multi-hazard environmental sensing, dual-band RF communication, live video surveillance, and a 4DOF robotic gripper arm into a single deployable platform — for under **₹10,000 (≈ USD 120)**.

Built for earthquakes, industrial gas leaks, structural fires, floods, and mine collapses, RescueBOT acts as an **autonomous advance scout** — collecting telemetry, broadcasting GPS coordinates, streaming live video, and physically delivering emergency payloads to survivors.

> *"A rover that goes where humans can't — so fewer humans have to."*

---

## ⚡ Key Capabilities

| Capability | Implementation |
|:---|:---|
| 🧪 **Toxic Gas Detection** | MQ-2 analog sensor → ADC threshold alert |
| 🔥 **Dual Flame Detection** | KY-026 IR × 2 → 130°+ arc fire sensing |
| 📳 **Seismic / Vibration Monitor** | SW-420 → aftershock & structural impact detection |
| 📏 **Obstacle Avoidance** | HC-SR04 ultrasonic + SG90 auto-sweep (0°–180°) |
| 📐 **Platform Tilt / Tipover** | MPU6050 6-axis IMU → pitch/roll alert |
| 📍 **Real-Time GPS Tracking** | u-blox NEO-6M → lat/lon on live dashboard map |
| 📷 **Live Video Surveillance** | ESP32-CAM OV2640 → MJPEG stream @ ~22 FPS |
| 🎮 **Dual-Joystick Remote Control** | nRF24L01+ PA+LNA @ 2.4 GHz, 250 kbps, Ch. 108 |
| 📡 **Long-Range Telemetry** | SX1278 LoRa @ 433 MHz → 350 m urban / 1.2 km LOS |
| 🦾 **4DOF Robotic Gripper Arm** | MG90S + SG90 servos → 100–150g payload delivery |
| 🚨 **Local Alarms** | Red LED + 2500 Hz active buzzer → standalone alerts |
| 🛡️ **Drive Fail-Safe** | Auto-halt within 500 ms of RF signal loss |

---

## 🏗️ System Architecture

```
[OPERATOR REMOTE CONTROLLER]              [RESCUEBOT ROVER CHASSIS]
┌─────────────────────────────┐           ┌──────────────────────────────────┐
│  Arduino Nano TX             │           │  Arduino Nano RX                 │
│  ├─ Car Joystick (A2, A3)   │  2.4 GHz  │  ├─ L298N → 4× BO DC Motors     │
│  ├─ Arm Joystick (A0, A1)   │ nRF Link  │  └─ MG90S/SG90 Arm Servos       │
│  └─ Grip Toggle (Pin 3)     ├──────────►│                                  │
│  nRF24L01+ PA+LNA           │           │  ESP32 Telemetry Node            │
└─────────────────────────────┘           │  ├─ MQ-2, SW-420, KY-026×2      │
                                          │  ├─ HC-SR04, MPU6050, NEO-6M     │
[BASE OPERATIONS STATION]                 │  ├─ Buzzer, LEDs                 │
┌─────────────────────────────┐  433 MHz  │  └─ SX1278 LoRa TX               │
│  Web Dashboard + PC         │ LoRa Link │                                  │
│  ESP32 LoRa Receiver       ◄├──────────┤  ESP32-CAM Node                  │
│  GPS Map + Sensor Gauges   ◄├──Wi-Fi───┤  └─ OV2640 MJPEG Port 81        │
└─────────────────────────────┘           └──────────────────────────────────┘
```

---

## 📁 Repository Structure

```
RescueBOT/
├── firmware/
│   ├── lora_module/          # ESP32 LoRa TX (sensor telemetry) + RX (base station)
│   ├── nrf_communication/    # Arduino Nano TX (remote) + RX (chassis controller)
│   ├── cam_module/           # ESP32-CAM MJPEG stream server
│   ├── sensor_module/        # Sensor test sketches
│   ├── gps_module/           # GPS standalone test
│   └── libraries/            # Local library copies (RF24, LoRa, TinyGPS++, etc.)
├── circuit_diagram/
│   ├── circuit_explanation.md  # Full system wiring & functional block docs
│   ├── pin_connections.md      # Pin mapping tables for all 5 boards
│   └── circuit_schematic.jpeg  # Visual wiring schematic
├── docs/                     # 14-file technical documentation suite
│   ├── 01_Project_Overview.md
│   ├── 02_Problem_Statement.md
│   ├── 03_Objectives.md
│   ├── 04_Features.md
│   ├── 05_Innovation_and_USP.md
│   ├── 06_Working_Principle.md
│   ├── 07_System_Architecture.md
│   ├── 08_Tech_Stack.md
│   ├── 09_Implementation.md
│   ├── 10_Testing_and_Results.md
│   ├── 11_Challenges_and_Solutions.md
│   ├── 12_Future_Scope.md
│   ├── 13_Team_Details.md
│   └── 14_References.md
├── hardware/                 # Hardware specs, cost analysis
├── media/                    # Logo, component images, pin diagrams
├── demo/                     # Prototype photos and demo videos
├── hackathon_gallery/        # ZYRO 2026 event photos
├── presentations/            # PPTX + PDF presentations
├── website/                  # Web dashboard source (HTML/CSS/JS)
├── README.md
├── INSTALLATION.md           # Quick start guide
├── HARDWARE_SETUP.md         # Full hardware assembly guide
├── SOFTWARE_SETUP.md         # Firmware flashing & dashboard setup
├── CONTRIBUTING.md           # Contribution guidelines
├── CHANGELOG.md              # Version history
├── ROADMAP.md                # Future development plans
├── FAQ.md                    # Frequently asked questions
├── TROUBLESHOOTING.md        # Known issues & fixes
├── CODE_OF_CONDUCT.md        # Community standards
├── SECURITY.md               # Security policy & vulnerability reporting
└── LICENSE                   # MIT License
```

---

## 🚀 Quick Start

### Prerequisites
- Arduino IDE 2.x with ESP32 board core and Arduino AVR core installed
- Python 3.x (for dashboard serial bridge)
- A modern browser (Chrome / Firefox)

### 1. Clone the Repository
```bash
git clone https://github.com/Prolayjit-B14/RescueBOT.git
cd RescueBOT
```

### 2. Flash the Firmware
```bash
# Flash to ESP32 Telemetry Node
# Open firmware/lora_module/tx-lora.ino in Arduino IDE
# Board: ESP32 Dev Module | Speed: 921600

# Flash to Arduino Nano RX (chassis)
# Open firmware/nrf_communication/rx.ino
# Board: Arduino Nano | Processor: ATmega328P (Old Bootloader)

# Flash to ESP32-CAM
# Open firmware/cam_module/production.ino
# Board: AI Thinker ESP32-CAM
```

### 3. Launch the Dashboard

> **Note:** The main web dashboard source code is hosted and maintained in a separate repository:
> 🌐 **[Diaster_Rover_Dashboard](https://github.com/Prolayjit-B14/Diaster_Rover_Dashboard)**

For local testing using the bundled version:
```bash
cd website
npm install
npm run dev
```

→ Full setup guide: **[INSTALLATION.md](INSTALLATION.md)**
→ Hardware wiring: **[HARDWARE_SETUP.md](HARDWARE_SETUP.md)**
→ Firmware config: **[SOFTWARE_SETUP.md](SOFTWARE_SETUP.md)**

---

## 📄 Documentation

| File | Description |
|:---|:---|
| [01 — Project Overview](docs/01_Project_Overview.md) | What RescueBOT is, specs, use cases |
| [02 — Problem Statement](docs/02_Problem_Statement.md) | Why this project matters |
| [03 — Objectives](docs/03_Objectives.md) | Technical goals & benchmarks |
| [04 — Features](docs/04_Features.md) | Complete capability breakdown |
| [05 — Innovation & USP](docs/05_Innovation_and_USP.md) | Market differentiation |
| [06 — Working Principle](docs/06_Working_Principle.md) | Data flow & operational logic |
| [07 — System Architecture](docs/07_System_Architecture.md) | Hardware & software layers |
| [08 — Tech Stack](docs/08_Tech_Stack.md) | Components, libraries, tools |
| [09 — Implementation](docs/09_Implementation.md) | Build process & firmware |
| [10 — Testing & Results](docs/10_Testing_and_Results.md) | Validated metrics & test data |
| [11 — Challenges & Solutions](docs/11_Challenges_and_Solutions.md) | Engineering hurdles & fixes |
| [12 — Future Scope](docs/12_Future_Scope.md) | Roadmap & upgrade plans |
| [13 — Team Details](docs/13_Team_Details.md) | Team BOT THINGS |
| [14 — References](docs/14_References.md) | Datasheets & research citations |

---

## 🧰 Hardware at a Glance

| Component | Model | Role |
|:---|:---|:---|
| Microcontroller × 2 | ESP32-WROOM-32 | Telemetry + Base Receiver |
| Camera | ESP32-CAM (OV2640) | Live video surveillance |
| Controller × 2 | Arduino Nano V3 (ATmega328P) | Remote TX + Motor/Servo RX |
| Control RF | nRF24L01+ PA+LNA | 2.4 GHz joystick control |
| Telemetry RF | SX1278 LoRa RA-02 | 433 MHz sensor uplink |
| Gas Sensor | MQ-2 | LPG, CH₄, CO, Smoke |
| Flame Sensor | KY-026 × 2 | IR fire detection |
| Vibration | SW-420 | Seismic / impact sensing |
| Distance | HC-SR04 | Obstacle avoidance |
| IMU | GY-521 MPU6050 | Tilt / tipover detection |
| GPS | u-blox NEO-6M | Real-time coordinate tracking |
| Motor Driver | L298N Dual H-Bridge | 4× DC motor control |
| Servos | MG90S × 2 + SG90 × 2 | 4DOF robotic arm |

**Total BOM Cost: ≤ ₹9,970 (≈ USD 120)**

---

## 📊 Performance Metrics

| Metric | Result |
|:---|:---|
| Control latency (joystick → motor) | ~35 ms |
| nRF24L01+ control range | ~200 m LOS |
| LoRa telemetry range (urban) | ~350 m |
| LoRa telemetry range (LOS) | ~1.2 km |
| LoRa wall penetration | 3 concrete walls |
| Video stream FPS (VGA + PSRAM) | ~22 FPS |
| Camera stream latency | 110–160 ms |
| Fail-safe halt time | < 500 ms |
| GPS cold-start fix time | ~38 s |
| BOM cost | ≤ ₹9,970 |

---

## 🏆 Hackathon

> **ZYRO 2026** · Open Innovation Track · Kalyani Government Engineering College (KGEC)
>
> Live demo included: telemetry streaming, gas/flame hazard alerts, real-time GPS map, MJPEG video feed, and robotic arm payload pickup — all validated in front of judges.

---

## 👥 Team BOT THINGS

| Member | Role |
|:---|:---|
| **Prolayjit Biswas** | Team Lead · Embedded Systems · Hardware Architecture |
| **Arghya Roy** | Software · Dashboard · IoT Integration |
| **Papon Chowdhury** | Robotics · Mechanical Assembly · Servo Calibration |
| **Subhajit Halder** | Research · Documentation · Testing |

---

## 🤝 Contributing

We welcome contributions! Please read [CONTRIBUTING.md](CONTRIBUTING.md) before opening a PR.

- 🐛 **Bug Reports** → [Open an Issue](https://github.com/Prolayjit-B14/RescueBOT/issues)
- 💡 **Feature Requests** → [Start a Discussion](https://github.com/Prolayjit-B14/RescueBOT/discussions)
- 🔒 **Security Issues** → [SECURITY.md](SECURITY.md) (do **not** use public Issues)

---

## 📜 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for full text.

You are free to use, modify, and distribute this project for personal, educational, and commercial purposes with attribution.

---

<div align="center">

Made with ❤️ for disaster survivors everywhere.

**⭐ Star this repo if RescueBOT inspires you!**

</div>
