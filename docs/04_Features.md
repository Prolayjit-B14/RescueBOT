# 🔌 04 — Features: Complete System Capabilities & Integration Details

---

## Overview

RescueBOT integrates six distinct feature categories across five hardware subsystems. Each feature described below is implemented and validated in the prototype firmware and physical hardware. Feature specifications are derived directly from the active firmware (`tx-lora.ino`, `rx-lora.ino`, `tx.ino`, `rx.ino`, `production.ino`) and confirmed through bench testing.

---

## 1. Multi-Sensor Environmental Telemetry Array

All six environment-monitoring sensors are consolidated onto the **Telemetry ESP32 Node**, enabling a single-board sensor fusion loop that aggregates, thresholds, and serializes all readings every 1000 ms.

### 1.1 Toxic Gas & Smoke Detection (MQ-2)
- **Sensor:** MQ-2 analog gas sensor — sensitive to LPG, Methane, Smoke, Hydrogen, Alcohol, Propane, CO.
- **Interface:** Analog ADC on GPIO 27 (12-bit, 0–4095 scale at 0–3.3V).
- **Firmware Logic:** Active-high calibration detection. If raw ADC reading is >3000 in clean air (indicating an active-high comparator board variant), firmware automatically inverts: `GasValue = 4095 - RawValue`.
- **Alert Threshold:** `GasValue > 1800`.
- **Validated Output:** During butane gas exposure test, reading peaked at 2350 (Critical), triggering full alert chain.

### 1.2 Structural Vibration & Seismic Impact Monitoring (SW-420)
- **Sensor:** SW-420 vibration sensor (spring-based tilt/shock mechanism).
- **Interface:** Analog ADC on GPIO 34.
- **Firmware Logic:** Raw value mapped to 0–100% intensity scale for human-readable telemetry output.
- **Alert Threshold:** `VibrationValue > 1500`.
- **Use Case:** Detects aftershock events in earthquake rubble, structural impact from secondary collapses, and physical shock to the UGV chassis.

### 1.3 Dual-Axis Infrared Flame Detection (KY-026 × 2)
- **Sensors:** Two KY-026 IR flame sensor modules (phototransistor + comparator).
- **Interface:** Digital GPIO 25 (Flame Sensor 1) and GPIO 33 (Flame Sensor 2) — active-LOW outputs.
- **Spectral Range:** Sensitive to IR wavelengths approximately 760 nm–1100 nm (open flame emission peak).
- **Firmware Logic:** OR logic — `fire = (flame1 == LOW || flame2 == LOW)`. Dual sensors provide 130°+ forward detection arc coverage.
- **Alert Threshold:** Either sensor pulls LOW.
- **Note:** GPIO 33 was re-assigned from GPIO 26 (original conflict with LoRa RX DIO0 line) in firmware revision.

### 1.4 Forward Obstacle Detection & Collision Avoidance (HC-SR04)
- **Sensor:** HC-SR04 ultrasonic distance sensor.
- **Interface:** Trigger on GPIO 2 (Output), Echo on GPIO 4 (Input).
- **Operating Range:** 2 cm – 400 cm (effective in-field range: ~2 cm – 200 cm).
- **Firmware Logic:** 10 µs trigger pulse; `pulseIn(ECHO_PIN, HIGH, 30000)` (30 ms timeout prevents loop hang on missed echo); distance calculated as:
  ```
  Distance (cm) = (Echo_Duration_µs × 0.034) / 2
  ```
- **Alert Threshold:** `Distance < 20 cm`.
- **Sweep Feature:** A dedicated SG90 servo (Arduino Nano RX, Pin A3) pans the HC-SR04 between 0° and 180° continuously in 1° steps every 15 ms, providing a full forward hemisphere obstacle sweep.

### 1.5 Platform Orientation & Tilt Stability (MPU6050)
- **Sensor:** GY-521 MPU6050 6-axis IMU (3-axis accelerometer + 3-axis gyroscope).
- **Interface:** I2C bus — SDA GPIO 21, SCL GPIO 22. I2C address: 0x68. Pull-up resistors: 4.7 kΩ on both lines.
- **Library:** `Adafruit_MPU6050` + `Adafruit_Sensor` (Unified Sensor API).
- **Firmware Logic:** Reads acceleration on X, Y, Z axes (m/s²). Pitch and Roll angles inferred from accelerometer gravity vector.
- **Alert Threshold:** `|ax| > 15 m/s²` OR `|ay| > 15 m/s²` — indicates >~57° tilt, indicating tipover risk.
- **Telemetry Output:** AX, AY, AZ values transmitted in every LoRa packet for dashboard visualization.

### 1.6 Real-Time GPS Coordinate Tracking (NEO-6M)
- **Module:** u-blox NEO-6M GPS module with active ceramic patch antenna.
- **Interface:** UART2 Hardware Serial — RX2 on GPIO 16, TX2 on GPIO 17 at 9600 baud, 8N1.
- **Library:** `TinyGPSPlus` (Mikal Hart) for NMEA sentence parsing.
- **Data Fields Parsed:** Latitude, Longitude, Altitude, Speed (km/h), Course, HDOP, Satellite count.
- **Firmware Logic:** Continuous character ingestion with `gps.encode(gpsSerial.read())`. Valid coordinates are added to the LoRa packet when `gps.location.isValid()` returns true. Invalid fix returns `LAT=0.0, LON=0.0`.
- **Cold Start Fix Time:** ~30 seconds in open sky conditions (per NEO-6M datasheet).

---

## 2. Standalone Visual Surveillance System (ESP32-CAM)

### 2.1 Independent MJPEG Live Video Stream
- **Module:** ESP32-CAM AI-Thinker with OV2640 2MP CMOS sensor.
- **Firmware:** `cam_module/production.ino` — standalone HTTP server.
- **Stream Endpoint:** `http://<camera_ip>:81/stream` — Motion JPEG (MJPEG).
- **Resolution Logic:**
  - With PSRAM detected: **VGA (640×480)**, dual framebuffers → ~22 FPS observed.
  - Without PSRAM (QVGA fallback): **320×240** for low-latency single framebuffer mode.
- **Camera Latency:** 110–160 ms over local Wi-Fi (tested on 2.4 GHz 802.11n).

### 2.2 PWM Flash LED Control
- **Flash LED:** High-power white LED on GPIO 4, controlled via LEDC PWM driver.
- **Firmware Compatibility:** Code handles both ESP32 Arduino Core v2.x (`ledcSetup()` + `ledcAttachPin()`) and v3.x (`ledcAttach()`) API differences automatically.
- **Use Case:** Enables night-mode surveillance in low-light disaster environments (collapsed buildings, night operations).

### 2.3 System Isolation for Stability
- **Critical Design Decision:** The ESP32-CAM is deliberately isolated from all telemetry sensors. GPIO 16 on the ESP32-CAM is hardwired to the OV2640 PSRAM clock signal and **cannot** be used as a general-purpose pin. If any sensor (e.g., HC-SR04 Echo) is wired to GPIO 16, it causes PSRAM clock conflicts leading to camera freeze, frame corruption, and system crashes.
- **Solution Implemented:** All sensors reside exclusively on the dedicated Telemetry ESP32 board. The ESP32-CAM has zero physical connections to the sensor suite.

---

## 3. Drive Control & Actuation System

### 3.1 Dual-Joystick Remote Controller (Arduino Nano TX)
- **Joysticks:** Two dual-axis analog joysticks — Car joystick (A2=VRX, A3=VRY) and Arm joystick (A0=VRX, A1=VRY).
- **Input Range:** 0–1023 raw ADC (0V–5V).
- **Smoothing Filter:** Exponential moving average applied continuously:
  ```
  smooth = (raw - smooth) × 0.35 + smooth
  ```
  This filters high-frequency mechanical jitter without adding delay.
- **Deadzones:** Car joystick: ±90 units around center (512). Arm joystick: ±70 units. Values within deadzone are treated as zero to prevent motor/servo creep.
- **Gripper Control:** Digital Pin 3 with `INPUT_PULLUP`. Toggle switch alternates gripper between OPEN (0°) and CLOSED (90°).
- **Packet Structure:** `DataPacket { carX, carY, armX, armY, carMove, armMove, grip }` — 9-byte RF payload.

### 3.2 RF Control Link (nRF24L01+ PA+LNA)
- **Modules:** nRF24L01+ with PA+LNA amplifier and SMA antenna (both TX and RX units).
- **Protocol:** 250 kbps data rate (lower rate = higher sensitivity = extended range).
- **Channel:** 108 — above standard 2.4 GHz Wi-Fi channels (1–13) to minimize interference.
- **Library:** TMRh20 `RF24`.
- **Transmission:** Reliable auto-ACK packet mechanism with up to 3 retries.

### 3.3 Drive Motor Control (L298N + 4× BO Motors)
- **Driver:** L298N Dual H-bridge (handles up to 2A per channel).
- **Motors:** 4× BO plastic geared DC motors (3V–6V, ~150 RPM at 6V).
- **Pin Mapping:** IN1 (Pin 2), IN2 (Pin 3), IN3 (Pin 9), IN4 (Pin 10), ENA (Pin 5 PWM), ENB (Pin 6 PWM).
- **Speed Control:** Standard drive speed = PWM 120 (47% duty cycle). Turn speed = PWM 100 (39% duty cycle) for better traction.
- **Steering Logic:** Differential drive — left/right motor direction inversion creates turning.

### 3.4 4DOF Robotic Arm (MG90S + SG90 Servos)
| Joint | Servo | MCU Pin | Range | Step | Function |
|:---|:---|:---|:---|:---|:---|
| Shoulder | MG90S (metal gear) | A2 | 0°–120° | 5° | Vertical arm lift |
| Elbow | MG90S (metal gear) | A5 | 0°–120° | 5° | Arm bend/extension |
| Gripper | SG90 (plastic gear) | A4 | 0°–90° | Toggle | Payload claw open/close |
| Sweep Pan | SG90 (plastic gear) | A3 | 0°–180° | 1° auto | Continuous sensor sweep |

- MG90S torque: 1.8 kg·cm at 4.8V (datasheet spec) — sufficient for 100–150g payloads at moderate arm extension.
- Sweep servo runs an autonomous panning subroutine: increments/decrements 1° every 15 ms, independent of joystick input, to continuously sweep the HC-SR04 sensor.

---

## 4. Dual-Band Wireless Communication System

| Band | Protocol | Hardware | Data Type | Range |
|:---|:---|:---|:---|:---|
| 2.4 GHz | nRF24L01+ (250 kbps, Ch. 108) | Nordic Semi nRF24L01+ PA+LNA | Control packets (9 bytes) | ~200 m LOS |
| 433 MHz | LoRa SX1278 | Ai-Thinker RA-02 module | Sensor telemetry ASCII string | ~350 m urban / 1.2 km LOS |
| 2.4 GHz Wi-Fi | IEEE 802.11 b/g/n | ESP32-CAM integrated | MJPEG video stream | ~30–50 m LAN |

---

## 5. Local Safety & Alert System

### 5.1 Hardware Alarm Output
- **Red Alert LED:** GPIO 12 — HIGH when any threshold is breached.
- **Green Status LED:** GPIO 14 — HIGH when all readings are nominal.
- **Active Buzzer:** GPIO 15 — HIGH when any threshold is breached. Produces ~2500 Hz audible tone for acoustic location signaling.

### 5.2 Autonomous Local Evaluation
The alert logic runs continuously and independently of LoRa link status:

```
IF (Gas > 1800) OR (Vibration > 1500) OR (Flame1 == LOW) OR (Flame2 == LOW)
   OR (Distance < 20 cm) OR (|ax| > 15) OR (|ay| > 15):
      → alert = true
      → RED_LED = HIGH, BUZZER = HIGH, GREEN_LED = LOW
      → LoRa ALERT flag = 1
ELSE:
      → alert = false
      → RED_LED = LOW, BUZZER = LOW, GREEN_LED = HIGH
      → LoRa ALERT flag = 0
```

### 5.3 Drive Fail-Safe
If the Arduino Nano RX does not receive an nRF24L01+ packet within **500 ms**, it immediately sets all motor PWM outputs to 0 and halts the drive system. This prevents uncontrolled rover movement in the event of operator disconnection, radio jamming, or power failure at the transmitter.

---

## 6. Web Dashboard & Telemetry Monitoring

The base station ESP32 receives LoRa packets and bridges them via USB-Serial to the operator's laptop. A web dashboard (HTML/CSS/JavaScript with optional Node.js backend) parses the ASCII telemetry string and renders:

- **Live Sensor Gauges:** Gas PPM, vibration intensity, distance reading, IMU pitch/roll.
- **Alert Status Panel:** Per-sensor RED/GREEN status indicators.
- **GPS Map View:** UGV latitude/longitude plotted on Google Maps or OpenStreetMap (Leaflet.js).
- **Video Panel:** Embedded `<img src="http://<cam_ip>:81/stream">` MJPEG stream in dashboard page.
- **Telemetry Log:** Scrollable timestamped text log of all LoRa packets received.

---

*Previous: [03 — Objectives ←](./03_Objectives.md) | Next: [05 — Innovation & USP →](./05_Innovation_and_USP.md)*
