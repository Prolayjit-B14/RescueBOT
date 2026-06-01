# 🚀 12 — Future Scope: Scalability Roadmap & Next-Generation Upgrades

---

## Overview

RescueBOT's prototype demonstrates a functional and validated disaster response UGV at a sub-₹10,000 price point. The modular hardware architecture and open-source firmware base are deliberately designed to accommodate incremental capability upgrades without requiring full redesign. This document outlines the concrete engineering roadmap for future versions, prioritized by implementation complexity and expected operational impact.

---

## 1. Phase 1 — Near-Term Hardware Upgrades (Prototype → Field-Ready)

### 1.1 OLED Local Telemetry Display

**Current State:** The SSD1306 OLED display (0.96", I2C address 0x3C) is included in the tech stack but not yet active in the primary firmware.

**Implementation Plan:**
- Connect SSD1306 to the shared I2C bus (GPIO 21/22) alongside the MPU6050 (address 0x68).
- Implement a rotating display screen: Page 1 — Gas/Vibration; Page 2 — Flame/Distance; Page 3 — GPS Coordinates; Page 4 — IMU Pitch/Roll.
- Add `Adafruit_SSD1306` and `Adafruit_GFX` libraries to tx-lora.ino firmware.

**Impact:** Provides local sensor readout independent of base station connectivity — useful when the UGV operator is in direct visual range of the rover.

### 1.2 PIR Passive Infrared Human Presence Detection

**Current State:** PIR sensor is listed in the hardware BOM but not yet integrated into the telemetry firmware.

**Implementation Plan:**
- Connect PIR sensor (HC-SR501 or AM312) to an available ESP32 digital GPIO (e.g., GPIO 35 — input-only).
- Add PIR motion detection as a new telemetry field: `PIR=1` (motion detected) or `PIR=0` (no motion).
- Alert logic: PIR = 1 triggers a dedicated survivor-detected notification on the dashboard (green flashing indicator to distinguish from hazard alerts).

**Impact:** Adds passive human heat/motion detection capability to the sensor array, directly supporting the survivor localization mission objective.

### 1.3 Relay Module for Payload Deployment

**Current State:** A relay module is included in the hardware list but not yet implemented.

**Implementation Plan:**
- Connect a 5V relay module to a spare Arduino Nano RX GPIO pin.
- Add a third gripper toggle command (relay trigger) to the nRF24L01+ DataPacket struct.
- Use the relay to control a secondary mechanism — e.g., a small electromagnet or wire-release mechanism on the gripper arm for one-shot payload drop without requiring arm repositioning.

**Impact:** Extends delivery capability to precision drop of small payloads (emergency beacon devices, medicines) without requiring direct arm manipulation for release.

---

## 2. Phase 2 — Autonomous Navigation & Environment Mapping

### 2.1 2D LiDAR Integration for SLAM

**Target Component:** RPLiDAR A1 or LD19 Compact LiDAR (180°–360° scan, ~7,000 points/second).

**Technical Approach:**
- Mount LiDAR on the upper chassis, rotating freely.
- Connect via UART to an onboard SBC (Raspberry Pi Zero 2W or Orange Pi Zero 2).
- Implement 2D occupancy grid mapping using the **Google Cartographer** or **GMapping** ROS package.
- Publish map updates to the base station dashboard over the LoRa link (compressed occupancy grid delta packets).

**Engineering Consideration:** Full SLAM on the ESP32 is not feasible due to RAM constraints (~520 KB). A dedicated SBC running a lightweight Linux distribution handles the compute-intensive mapping loop while the ESP32 continues handling sensor telemetry and RF communication.

### 2.2 GPS Waypoint Navigation

**Technical Approach:**
- Integrate a GPS waypoint navigation engine on the SBC.
- Operator inputs target GPS coordinates on the dashboard.
- The SBC calculates bearing using the haversine formula and sends differential drive commands to the Arduino Nano RX via UART.
- NEO-6M GPS position is used as feedback for closed-loop navigation with a tolerance radius (e.g., ±3 m).

**Limitation:** GPS accuracy (CEP ~2.5 m with NEO-6M) limits autonomous navigation precision. For indoor environments, wheel odometry (via motor encoder counts) provides dead-reckoning backup.

### 2.3 Ultrasonic Array Expansion

**Current State:** Single HC-SR04 sweeping on a servo for forward hemisphere coverage.

**Upgrade Path:** Mount 3 additional HC-SR04 sensors (rear, left, right) for full 360° static obstacle detection, eliminating the need for the sweep servo and providing instantaneous multi-direction awareness.

---

## 3. Phase 3 — Edge AI & Computer Vision Upgrades

### 3.1 Human Silhouette Detection (Survivor Localization)

**Target Platform:** ESP32-S3 (with AI acceleration) or Raspberry Pi Zero 2W.

**Technical Approach:**
- Deploy a lightweight object detection model (MobileNet-SSD V2 INT8 or YOLOv5 Nano) optimized for edge inference.
- Model is trained on COCO dataset human class + fine-tuned on disaster scenario images.
- Inference runs directly on the camera image buffer.
- When a human silhouette is detected with confidence > 70%, a `HUMAN_DETECTED` alert with bounding box coordinates is transmitted via LoRa to the base dashboard.

**Estimated Hardware Cost Addition:** ~₹800–₹2,000 for Raspberry Pi Zero 2W.

**Research Reference:** Lightweight CNNs deployed on Raspberry Pi Zero 2W (BCM2710A1, 1 GHz) achieve 2–5 FPS for MobileNet-SSD — sufficient for stationary survivor detection.

### 3.2 Thermal Camera Overlay

**Target Component:** Flir Lepton 3.5 or MLX90640 32×24 thermal sensor.

**Technical Approach:**
- Mount MLX90640 (I2C, 32×24 pixels, 7 Hz update rate) adjacent to the OV2640 camera.
- Overlay thermal heat map gradient onto the OV2640 visual feed using OpenCV frame blending.
- Hot spots (body temperature 36–37°C) highlighted in orange/red overlay on dashboard stream.

**Impact:** Enables survivor detection through thin walls, smoke, and in total darkness — a critical capability for building fire and collapse rescue scenarios.

### 3.3 Gas Type Classification

**Technical Approach:**
- Replace single-point MQ-2 threshold with a multi-gas sensor array (MQ-2, MQ-4, MQ-7, MQ-135).
- Train a small classification model on multi-sensor voltage ratios to identify gas type (LPG, CO, NH₃, smoke) in addition to concentration level.
- Dashboard displays specific gas type identification rather than generic "gas alert."

---

## 4. Phase 4 — Aerial Integration & Swarm Robotics

### 4.1 Hybrid Ground-Aerial System

**Concept:** RescueBOT UGV serves as a mobile launch pad and charging station for a small aerial drone (e.g., DJI Tello-class or custom F450 quad).

**Technical Architecture:**
```
[RescueBOT UGV]
  ├─ Landing platform (top deck — retractable) with alignment guides
  ├─ Wireless charging pad (Qi 5W) for drone battery
  ├─ LoRa-to-MAVLink bridge: UGV relays drone GPS & status to base station
  └─ Mission coordination: Drone dispatched for aerial sweep when UGV is stationary
```

**Operational Value:**
- Drone provides **bird's-eye-view mapping** of rubble fields that the ground UGV cannot visually survey.
- Thermal camera on drone identifies survivor heat signatures from above, guiding the ground UGV to precise locations.
- Combined aerial + ground coverage of a disaster area is 5–10× faster than ground-only search.

### 4.2 Multi-Robot Swarm Coordination

**Concept:** Deploy 3–10 RescueBOT units simultaneously across a large disaster zone.

**Technical Architecture:**
- Each UGV assigned a unique LoRa address (0x01–0xFF) for packet identification at the base station.
- Base station aggregates telemetry from all units into a unified dashboard with per-unit GPS pins on the map.
- UGVs broadcast their GPS position over a shared LoRa channel; each unit updates a local obstacle map from received positions to prevent same-area overlap.
- Alert priority: if Unit 3 detects a HUMAN_DETECTED event, all other units navigate to within 10 m of Unit 3's GPS coordinates to assist.

**Estimated BOM per additional unit:** ₹9,970 — enabling a 5-unit swarm for under ₹50,000 (≈ USD 600).

---

## 5. Phase 5 — Cloud Infrastructure & Professional Deployment

### 5.1 Cloud Telemetry Dashboard

**Platform:** AWS IoT Core + DynamoDB + Amazon QuickSight, or open-source EMQX MQTT broker + Grafana.

**Data Pipeline:**
```
RescueBOT LoRa → Base Station ESP32 → Python MQTT Publisher
  → AWS IoT Core / EMQX → DynamoDB / InfluxDB
  → Grafana / QuickSight Dashboard → Relief Agency Web Portal
```

**Enables:**
- Real-time telemetry monitoring by remote emergency management teams.
- Historical sensor data logging for post-incident analysis.
- Automated SMS/email alerts on critical threshold breaches.

### 5.2 Mobile Application (Android / iOS)

**Stack:** React Native or Flutter cross-platform app.

**Features:**
- Real-time telemetry display (all sensor values).
- MJPEG video stream embedded in app.
- GPS map with rover position and hazard markers.
- Remote alert push notifications when `ALERT=1` is detected.
- One-touch teleoperation via on-screen virtual joystick (WebSocket control bridge to base station).

### 5.3 Weatherproofing & IP-Rated Enclosure

**Target Rating:** IP44 (splash-resistant for flood scenarios, dust-resistant for collapse/fire environments).

**Engineering Changes:**
- Replace acrylic chassis with aluminum or PETG 3D-printed enclosure.
- Conformal coat all PCBs with acrylic spray (e.g., MG Chemicals 419-340G).
- Route cable penetrations through rubber grommets.
- Gasket-seal all sensor apertures (mesh covers for MQ-2, optical windows for KY-026).

---

## 6. Future Scope Summary Timeline

| Phase | Upgrade | Estimated Cost Addition | Timeline |
|:---|:---|:---:|:---|
| Phase 1 | OLED Display, PIR Sensor, Relay Module | +₹500–₹800 | Immediate (next sprint) |
| Phase 2 | LiDAR SLAM, GPS Waypoints | +₹3,000–₹6,000 | 2–4 months |
| Phase 3 | Edge AI Human Detection, Thermal Camera | +₹2,000–₹5,000 | 3–6 months |
| Phase 4 | Drone Integration, Swarm Coordination | +₹8,000–₹20,000/unit | 6–12 months |
| Phase 5 | Cloud Dashboard, Mobile App, IP Enclosure | +₹5,000–₹15,000 | 6–18 months |

---

*Previous: [11 — Challenges & Solutions ←](./11_Challenges_and_Solutions.md) | Next: [13 — Team Details →](./13_Team_Details.md)*
