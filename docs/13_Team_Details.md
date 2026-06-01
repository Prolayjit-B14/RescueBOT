# 👥 13 — Team Details: BOT THINGS @ ZYRO 2026

---

## Team Introduction

**Team BOT THINGS** is a four-member interdisciplinary engineering team from Kalyani Government Engineering College (KGEC), West Bengal, India. The team was formed specifically for the **ZYRO 2026 Hackathon** under the Open Innovation track, where RescueBOT was conceived, built, and demonstrated.

The team brings together expertise spanning embedded systems development, robotics and mechanical engineering, full-stack software development, and research documentation. Each member took ownership of a specific technical domain while contributing to integration, testing, and the live demonstration.

---

## Team Identity

| | |
|:---|:---|
| **Team Name** | BOT THINGS |
| **Project** | RescueBOT — Autonomous Disaster Response & Rescue Robot |
| **Event** | ZYRO 2026 Hackathon |
| **Venue** | Kalyani Government Engineering College (KGEC), West Bengal, India |
| **Track** | Open Innovation |
| **GitHub Repository** | [Prolayjit-B14/RescueBOT](https://github.com/Prolayjit-B14/RescueBOT) |

---

## Team Members & Roles

### 1. Prolayjit Biswas — Team Lead & Embedded Systems Architect

| Field | Detail |
|:---|:---|
| **Role** | Team Lead / Embedded Systems Developer / Hardware Architect |
| **GitHub** | [@Prolayjit-B14](https://github.com/Prolayjit-B14) |
| **Core Expertise** | Embedded C/C++, ESP32 firmware, I2C/SPI/UART interfacing, LoRa communication |

**Contributions:**
- Overall system architecture design — defining the subsystem isolation principle (Telemetry ESP32 / Motor Nano / Camera node separation).
- Firmware development for `tx-lora.ino` — multi-sensor polling, threshold logic, LoRa packet assembly and transmission.
- Firmware development for `rx-lora.ino` — base station LoRa receiver and USB-Serial bridge.
- Pin assignment and GPIO conflict resolution (discovered and resolved ESP32-CAM GPIO 16 / PSRAM conflict; Flame Sensor GPIO 26 / LoRa DIO0 conflict).
- Power architecture design — rail separation, decoupling capacitor placement, and brownout mitigation.
- Library integration: RF24, LoRa (Sandeep Mistry), TinyGPSPlus, Adafruit MPU6050.
- MQ-2 auto-calibration logic implementation (active-high/active-low board variant detection).
- Primary circuit breadboarding and sensor wiring on the Telemetry ESP32 node.

---

### 2. Arghya Roy — Software Engineer & Dashboard Developer

| Field | Detail |
|:---|:---|
| **Role** | Software / IoT Integration / Web Dashboard |
| **Core Expertise** | HTML/CSS/JavaScript, Python, Node.js, WebSocket, REST APIs, GPS map integration |

**Contributions:**
- Web dashboard design and implementation — sensor gauge panels, alert status indicators, telemetry log viewer.
- Python USB-Serial bridge script — reads LoRa receiver serial output and relays to dashboard via WebSocket.
- GPS map integration — Leaflet.js map with real-time UGV coordinate pin updates using parsed LAT/LON from telemetry packets.
- MJPEG video stream embedding in dashboard — `<img>` tag with ESP32-CAM stream URL.
- Telemetry packet parser (string splitting on commas and `=` delimiters → structured JavaScript object).
- Dashboard alert notification system — visual RED/GREEN per-sensor status panels.
- API design for future cloud integration (AWS IoT Core / EMQX MQTT bridge architecture planned).
- ESP32-CAM firmware configuration — Wi-Fi SSID/password setup, MJPEG stream server testing and frame rate verification.

---

### 3. Papon Chowdhury — Robotics Engineer & Mechanical Build Lead

| Field | Detail |
|:---|:---|
| **Role** | Robotics / Mechanical Assembly / Hardware Integration |
| **Core Expertise** | Mechanical assembly, servo calibration, robotic arm kinematics, chassis integration |

**Contributions:**
- Full mechanical assembly of the 4-wheel acrylic chassis — motor mounting, wheel attachment, layer bolt fastening.
- Robotic arm construction — servo bracket assembly, shoulder/elbow/gripper servo mounting, arm linkage fabrication.
- Sweep servo bracket design and mounting for HC-SR04 panning mechanism.
- Servo mechanical calibration — physical range verification for Shoulder (0°–120°), Elbow (0°–120°), Gripper (0°–90°) to ensure arm movement stays within safe mechanical limits.
- Prototype PCB (veroboard) layout for Arduino Nano RX — L298N connections, servo signal wire routing, nRF24L01+ placement.
- Cable management across chassis layers — wire bundling, strain relief at servo joints, connector routing for clean assembly.
- ESP32-CAM housing bracket fabrication and camera orientation alignment.
- Sensor placement optimization — HC-SR04 position for unobstructed forward sweep, MQ-2 positioning for airflow access, GPS module antenna skyward orientation.
- Field testing harness — simulated rubble course, tilted surface for IMU testing, payload weight preparation for arm tests.

---

### 4. Subhajit Halder — Research Analyst & Technical Documentation Lead

| Field | Detail |
|:---|:---|
| **Role** | Research / Documentation / Component Verification |
| **Core Expertise** | Technical writing, datasheet research, component specification verification, test documentation |

**Contributions:**
- Primary author of all 14 technical documentation files in the `/docs` directory.
- Component research and datasheet verification for all sensors, transceivers, and microcontrollers.
- Compilation of IEEE and academic references on disaster response robotics, sub-GHz radio propagation, and autonomous rescue systems.
- Test specification design — defining pass/fail criteria for each of the 12 subsystem test procedures.
- Test result recording during prototype validation sessions.
- Bill of Materials (BOM) compilation and cost analysis research (components\_list.png, cost\_analysis.png).
- Hardware specification documentation (hardware\_specification.png).
- Pin connection tables (circuit\_diagram/pin\_connections.md) — verification against firmware GPIO assignments.
- GitHub repository structure organization — README, CONTRIBUTING.md, folder layout.
- Hackathon presentation material preparation — slide content, project overview summaries, demo script.

---

## Team Contribution Overview

| Domain | Prolayjit | Arghya | Papon | Subhajit |
|:---|:---:|:---:|:---:|:---:|
| System Architecture Design | ✅ Lead | 🤝 Support | 🤝 Support | 📝 Doc |
| Embedded Firmware (ESP32) | ✅ Lead | — | — | 📝 Doc |
| Embedded Firmware (Arduino Nano) | ✅ Lead | — | 🤝 Support | 📝 Doc |
| Circuit Design & GPIO Mapping | ✅ Lead | 🤝 Support | 🤝 Support | 📝 Doc |
| Power Architecture | ✅ Lead | — | 🤝 Support | 📝 Doc |
| Web Dashboard & Frontend | 🤝 Support | ✅ Lead | — | 📝 Doc |
| Serial Bridge & Backend | 🤝 Support | ✅ Lead | — | 📝 Doc |
| GPS Map Integration | — | ✅ Lead | — | 📝 Doc |
| ESP32-CAM Configuration | ✅ Co-Lead | ✅ Co-Lead | — | 📝 Doc |
| Mechanical Assembly | — | — | ✅ Lead | — |
| Robotic Arm Build & Calibration | 🤝 Support | — | ✅ Lead | 📝 Doc |
| Chassis Wiring & Cable Mgmt | 🤝 Support | — | ✅ Lead | — |
| Technical Documentation | 🤝 Support | 🤝 Support | 🤝 Support | ✅ Lead |
| Component Research & BOM | — | 🤝 Support | 🤝 Support | ✅ Lead |
| Testing & Validation | ✅ Co-Lead | 🤝 Support | ✅ Co-Lead | ✅ Lead |
| Presentation & Demo Script | 🤝 Support | 🤝 Support | 🤝 Support | ✅ Lead |

*Legend: ✅ Lead — Primary owner | 🤝 Support — Active contributor | 📝 Doc — Documentation responsibility*

---

## Collaboration Workflow

```
Weekly Sprint Cycle (Hackathon Preparation):
┌─────────────────────────────────────────────────────────┐
│  MONDAY        │ Hardware integration session (all 4)    │
│  TUESDAY       │ Firmware development (Prolayjit + Arghya)│
│  WEDNESDAY     │ Mechanical assembly (Papon + Prolayjit)  │
│  THURSDAY      │ Dashboard + testing (Arghya + Subhajit) │
│  FRIDAY        │ Documentation sprint (Subhajit)          │
│  WEEKEND       │ Full system integration test (all 4)     │
└─────────────────────────────────────────────────────────┘
```

**Version Control:** All firmware, circuit documentation, and project files are managed in a Git repository with feature-branch development, pull request review, and main-branch merge workflow.

**Communication:** Daily async updates via team WhatsApp group for build status. Critical integration changes communicated synchronously before merging to main.

---

## Acknowledgements

The team extends gratitude to:
- **Kalyani Government Engineering College (KGEC)** — for hosting ZYRO 2026 and providing the venue and testing space.
- **ZYRO 2026 Organizing Committee** — for the Open Innovation track framework that enabled interdisciplinary project development.
- The open-source Arduino, Espressif, and Adafruit communities whose libraries, documentation, and examples formed the foundation of RescueBOT's firmware.

---

*Previous: [12 — Future Scope ←](./12_Future_Scope.md) | Next: [14 — References →](./14_References.md)*
