# 🎯 03 — Objectives: Project Goals, Technical Benchmarks & Success Criteria

---

## 1. Primary Project Goal

The overarching goal of RescueBOT is to **design, build, integrate, test, and demonstrate** a low-cost, modular, and technically robust Unmanned Ground Vehicle (UGV) capable of:

1. Entering hazardous disaster environments autonomously or via remote control.
2. Collecting and transmitting real-time multi-sensor environmental telemetry to a remote base station.
3. Providing live visual surveillance through an independent video stream.
4. Performing limited physical interaction via a 4DOF robotic gripper arm.
5. Operating reliably in GPS-denied, RF-obstructed, and electrically noisy environments.

All objectives are grounded in measurable, testable engineering specifications derived from component datasheets, firmware thresholds, and prototype test data.

---

## 2. Technical Objectives

### 2.1 Communication & Control

| Objective | Target Specification | Implementation |
|:---|:---|:---|
| Low-latency remote control link | Control latency ≤ 50 ms at ≤ 200 m LOS | nRF24L01+ PA+LNA at 250 kbps, Ch. 108 |
| Long-range telemetry link | Telemetry at ≥ 300 m urban / ≥ 1 km LOS | SX1278 LoRa at 433 MHz |
| Independent video feed | MJPEG stream at ≥ 15 FPS | ESP32-CAM OV2640 on Wi-Fi Port 81 |
| Dual-band RF isolation | No cross-interference between control & data | Control: 2.4 GHz, Telemetry: 433 MHz |
| Control fail-safe | Auto-halt within 500 ms of signal loss | Arduino Nano RX timeout → motor stop |

### 2.2 Environmental Sensing

| Objective | Sensor | Threshold / Accuracy |
|:---|:---|:---|
| Toxic gas detection | MQ-2 (GPIO 27, Analog) | Alert when ADC > 1800 (4095 scale) |
| Structural vibration monitoring | SW-420 (GPIO 34, Analog) | Alert when raw reading > 1500 |
| Dual-axis fire detection | KY-026 × 2 (GPIO 25, 33, Digital) | Alert when either sensor pulls LOW (IR ~760–1100 nm) |
| Obstacle collision avoidance | HC-SR04 (GPIO 2/4, Digital) | Alert when distance < 20 cm |
| Platform tilt & stability | MPU6050 (I2C GPIO 21/22) | Alert when |Pitch| or |Roll| > 15° |
| GPS coordinate tracking | NEO-6M (UART2 GPIO 16/17) | Valid fix: lat/lon parsed via TinyGPSPlus |

### 2.3 Actuation & Manipulation

| Objective | Component | Specification |
|:---|:---|:---|
| 4-wheel differential drive | 4× BO DC geared motors + L298N | PWM speed: 120 (drive) / 100 (turns) |
| Robotic arm shoulder joint | MG90S metal-gear servo (A2) | Range: 0°–120° in 5° steps |
| Robotic arm elbow joint | MG90S metal-gear servo (A5) | Range: 0°–120° in 5° steps |
| Gripper claw actuator | SG90 micro-servo (A4) | Open: 0°, Closed: 90° |
| Continuous sensor sweep | SG90 micro-servo (A3) | Pan: 0°–180°, step 1° every 15 ms |
| Payload capacity | Arm assembly | Estimated: 100–150 grams at full extension |

---

## 3. Safety & Reliability Objectives

### 3.1 Firmware Stability

- **Non-Blocking Execution Model:** All sensor polling, delay handling, and LED/buzzer control must use `millis()`-based timing guards. No `delay()` calls permitted in the main control loop. This ensures the UGV's control loop runs continuously without hang states.
- **Ultrasonic Timeout Guard:** `pulseIn(ECHO_PIN, HIGH, 30000)` — a 30 ms maximum echo wait prevents the control loop from freezing if the HC-SR04 fails to receive an echo (e.g., open space or sensor fault). Returns distance = 999 cm on timeout.
- **LoRa Packet Integrity:** Telemetry packets are serialized as ASCII key-value strings (`VIB=val,GAS=val,...`), providing human-readable error detection during debug monitoring.

### 3.2 Power Safety Architecture

- **Voltage Rail Isolation:** Drive motors are powered directly from the raw 7.4V–8.4V Li-ion battery. Microcontrollers and sensors operate on regulated 5V and 3.3V rails. This prevents motor startup current spikes from causing microcontroller brownouts.
- **Common Ground Rail:** All subsystem grounds are tied to a single shared reference to prevent floating ground-induced logic errors.
- **Decoupling Capacitors:** 100 nF ceramic decoupling capacitors placed at MQ-2 and SW-420 sensor power pins to reduce switching noise from adjacent motor currents.

### 3.3 Local Autonomous Alerting

The Telemetry ESP32 runs a fully local alert evaluation loop independent of the base station connection. Even if the LoRa link fails, the UGV's local Red LED (GPIO 12) and buzzer (GPIO 15) will activate when thresholds are breached. This provides:
- Audible location beacon for searchers near the UGV.
- Visual status indicator visible at short range.

---

## 4. Dashboard & Monitoring Objectives

| Feature | Objective |
|:---|:---|
| Telemetry visualization | Display all sensor values (Gas, Vibration, Flame, Distance, IMU, GPS) in a live dashboard UI |
| GPS map integration | Overlay UGV GPS coordinates on a map view (Google Maps API / OpenStreetMap) |
| Alert panel | Visual red/green status indicators per sensor channel on the dashboard |
| Video integration | Embed ESP32-CAM MJPEG stream as a live panel in the dashboard |
| Serial bridge | Forward LoRa receiver serial output to the dashboard via Node.js or Python bridge |

---

## 5. Scalability & Future Objectives

The prototype architecture is intentionally modular to support future hardware expansion without redesigning the core system:

### 5.1 Near-Term Expansion Targets
- **OLED Display Integration:** SSD1306 OLED (0.96", I2C address 0x3C) shares the I2C bus with MPU6050 (GPIO 21/22). The display can render local sensor readouts without base station connectivity.
- **Multi-Node Swarm Deployment:** Multiple UGVs can operate simultaneously using unique LoRa channel/frequency assignments and nRF24L01+ pipe addresses, with the base station aggregating telemetry from all units.

### 5.2 Long-Term Research Objectives
- **SLAM Navigation:** Integrate a 2D LiDAR scanner and implement Simultaneous Localization and Mapping (SLAM) for autonomous indoor navigation.
- **Edge AI Computer Vision:** Deploy a MobileNet-SSD object detection model on an ESP32-S3 or Raspberry Pi Zero 2W for real-time human detection in the video feed.
- **Aerial Hybrid System:** Mount a drone launch/recovery platform on the UGV chassis to enable combined ground-air reconnaissance.
- **Cloud Telemetry Pipeline:** Route base station data through an AWS IoT Core or EMQX cloud broker for remote monitoring by international relief agencies.

---

## 6. Hackathon & Portfolio Objectives

| Context | Success Criterion |
|:---|:---|
| ZYRO 2026 Demo | Live demonstration of telemetry, video, control, and arm manipulation |
| Technical Proposal | Engineering documentation complete for all 14 topic areas |
| Innovation Portfolio | Sub-₹10,000 BOM cost demonstrated against commercial alternatives |
| GitHub Repository | Complete firmware, circuit diagrams, and documentation published |

---

*Previous: [02 — Problem Statement ←](./02_Problem_Statement.md) | Next: [04 — Features →](./04_Features.md)*
