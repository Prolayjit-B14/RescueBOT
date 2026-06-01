# 🧪 10 — Testing & Results: Prototype Validation, Sensor Metrics & Performance Analysis

---

## Overview

The RescueBOT prototype underwent systematic subsystem testing followed by integrated system validation. All tests were conducted under controlled simulation conditions at the ZYRO 2026 Hackathon venue at Kalyani Government Engineering College (KGEC). The table below provides a comprehensive record of each test procedure, configured thresholds, observed outputs, and pass/fail status.

---

## 1. Subsystem Test Results

| # | Subsystem | Test Procedure | Threshold / Input Condition | Observed Output | Status |
|:---:|:---|:---|:---|:---|:---:|
| 1 | **Locomotion** | Drive in all four directions using joystick at full deflection (past ±90 deadzone). | Joystick offset > ±90 from center (512). | Forward, backward, left, and right steering all functional. Differential turning confirmed. PWM 120 (drive) and 100 (turns) applied correctly. | **✅ PASS** |
| 2 | **Fail-Safe** | Power on UGV in motion, then power off remote controller transmitter (Arduino Nano TX). | nRF24L01+ signal lost for > 500 ms. | Motors halted within measured 520 ms of signal loss. `ENA = ENB = 0` confirmed on scope. UGV stopped instantly without runaway motion. | **✅ PASS** |
| 3 | **Gas Detection** | Hold butane lighter (unlit, gas flowing) near MQ-2 sensor for 3 seconds. | Gas ADC reading > 1800. | Reading peaked at 2350 ADC units. Red LED activated, buzzer active (audible alarm). LoRa packet field: `GAS=2350,ALERT=1`. | **✅ PASS** |
| 4 | **Seismic Sensor** | Physically tap the UGV chassis base repeatedly with a closed fist (simulated impact). | Vibration raw ADC > 1500. | Vibration reading spiked to 1738 raw during hard impact. Red LED + buzzer active. LoRa packet: `VIB=1738,ALERT=1`. | **✅ PASS** |
| 5 | **Flame Detection** | Hold lit match at 5–8 cm distance from KY-026 array for 2 seconds. | Flame sensor GPIO pulls LOW (active-low). | FL1 OR FL2 pulled LOW. Immediate Red LED + buzzer trigger. LoRa packet: `FL1=0,ALERT=1`. Dual sensor ensures wide arc coverage. | **✅ PASS** |
| 6 | **Obstacle Avoidance** | Place cardboard sheet at 10 cm distance in front of HC-SR04. | Distance measured < 20 cm. | Measured distance: 10.2 cm. Red LED + buzzer triggered. LoRa packet: `DIST=10.20,ALERT=1`. Sweep servo continued auto-panning independently. | **✅ PASS** |
| 7 | **IMU Tilt Stability** | Tilt UGV manually beyond 60° from horizontal on one side. | |AX| or |AY| > 15 m/s² (≈ 57° tilt). | AX reading reached 16.2 m/s² during test. Alert state triggered: `ALERT=1`. Axis readings accurately reflect tilt direction. | **✅ PASS** |
| 8 | **GPS Tracking** | Place UGV in open outdoor area (KGEC campus). Allow 60s warm-up for satellite acquisition. | GPS cold start fix, valid lat/lon output. | GPS fix acquired after ~38 seconds (consistent with NEO-6M 30–45 s cold start spec). Parsed coordinates: `LAT=22.952311, LON=88.473862`. Telemetry packet updated correctly. | **✅ PASS** |
| 9 | **Video Stream** | Connect to ESP32-CAM Wi-Fi network. Open `http://<cam_ip>:81/stream` in Chrome browser. | Stable MJPEG stream at target FPS. | VGA (640×480) stream stable at ~22 FPS observed frame rate. Flash LED brightness adjustable via PWM control. No frame corruption or camera reset observed. | **✅ PASS** |
| 10 | **LoRa Penetration** | Place base station ESP32 receiver behind 3 stacked concrete block walls (total ~60 cm concrete). Run telemetry TX on rover side. | Successful packet reception through ≥ 3 concrete walls. | Reliable telemetry reception through 3 concrete walls confirmed. RSSI reported: −98 dBm. All sensor fields present and correctly formatted in received packet. | **✅ PASS** |
| 11 | **Robotic Arm Manipulation** | Use arm joystick to navigate shoulder and elbow joints to pick up a 100g test weight. | Payload pickup at up to 150g. | 100g test object successfully lifted and repositioned using shoulder + elbow joints. MG90S servos maintained position without thermal stall. Gripper toggle operated cleanly. | **✅ PASS** |
| 12 | **Dashboard Integration** | Connect base station to laptop via USB-Serial bridge. Launch dashboard in browser. | All sensor fields visible; GPS map pin updates; camera embedded. | All 10 telemetry fields rendered in dashboard. GPS map pin updated on coordinate receipt. MJPEG camera feed embedded and streaming simultaneously. | **✅ PASS** |

---

## 2. Sensor Telemetry Packet Samples

Actual telemetry strings recorded during testing:

### Normal Standby Conditions (No Hazards)
```
TX DATA:
VIB=12,GAS=412,FL1=1,FL2=1,DIST=85.00,AX=0.12,AY=-0.08,AZ=9.79,LAT=22.952311,LON=88.473862,ALERT=0
→ Status: OPTIMAL | Green LED: HIGH | Buzzer: SILENT
```

### Gas Hazard Alert Simulation (Butane Exposure)
```
TX DATA:
VIB=15,GAS=2350,FL1=1,FL2=1,DIST=85.00,AX=0.11,AY=-0.07,AZ=9.80,LAT=22.952311,LON=88.473862,ALERT=1
→ Status: CRITICAL | Red LED: HIGH | Buzzer: ACTIVE (2.5 kHz)
```

### Multi-Hazard Combined Alert (Gas + Flame)
```
TX DATA:
VIB=22,GAS=2867,FL1=0,FL2=1,DIST=85.00,AX=0.14,AY=-0.06,AZ=9.78,LAT=22.952311,LON=88.473862,ALERT=1
→ Status: CRITICAL | Gas: CRITICAL | Flame Sensor 1: ACTIVE
```

### Tilt Alert (Rover Tilted on Slope)
```
TX DATA:
VIB=50,GAS=320,FL1=1,FL2=1,DIST=120.00,AX=16.20,AY=-1.20,AZ=4.80,LAT=22.952311,LON=88.473862,ALERT=1
→ Status: TILT ALERT | AX exceeded 15 m/s² threshold
```

### Obstacle Alert (Object < 20 cm)
```
TX DATA:
VIB=14,GAS=308,FL1=1,FL2=1,DIST=10.20,AX=0.12,AY=-0.08,AZ=9.79,LAT=22.952311,LON=88.473862,ALERT=1
→ Status: OBSTACLE | Distance: 10.20 cm (threshold: 20 cm)
```

---

## 3. Quantitative Performance Metrics

| Metric | Measured Value | Target / Specification | Status |
|:---|:---:|:---:|:---:|
| Control loop latency (joystick → motor) | ~35 ms | ≤ 50 ms | **✅ Within spec** |
| nRF24L01+ control range (open area) | ~180 m | ≥ 100 m | **✅ Exceeds spec** |
| LoRa telemetry range (urban, obstructed) | ~350 m | ≥ 300 m | **✅ Meets spec** |
| LoRa telemetry range (line-of-sight) | ~1.2 km | ≥ 1 km | **✅ Meets spec** |
| LoRa wall penetration | 3 concrete walls | ≥ 2 walls | **✅ Exceeds spec** |
| LoRa RSSI (3-wall test) | −98 dBm | > −120 dBm | **✅ Adequate margin** |
| ESP32-CAM video FPS (VGA, PSRAM) | ~22 FPS | ≥ 15 FPS | **✅ Exceeds spec** |
| ESP32-CAM stream latency | 110–160 ms | ≤ 300 ms | **✅ Within spec** |
| Telemetry packet rate | 1 Hz (1000 ms cycle) | ≥ 0.5 Hz | **✅ Meets spec** |
| Fail-safe halt time (signal loss) | ~520 ms | ≤ 600 ms | **✅ Within spec** |
| GPS cold start fix time | ~38 s | < 60 s | **✅ Within spec** |
| Gas alert response (local alarm) | < 50 ms | ≤ 100 ms | **✅ Within spec** |
| Flame detection response (local alarm) | < 50 ms | ≤ 100 ms | **✅ Within spec** |
| Robotic arm payload capacity | 100 g tested | ≥ 100 g | **✅ Meets spec** |

---

## 4. Sensor Calibration Notes

### MQ-2 Gas Sensor
The MQ-2 requires a **preheat period of approximately 20 minutes** before readings stabilize. During bench testing, sensors were preheated for 30 minutes before calibration baselines were recorded. The ADC threshold of 1800 was chosen based on observed clean-air baseline (400–500 ADC) and butane exposure peak (~2350 ADC), providing a safety margin with minimal false positives.

### HC-SR04 Ultrasonic
The HC-SR04 shows ±3 mm accuracy per datasheet in ideal conditions. In field testing against flat cardboard, accuracy was ±5 mm at distances up to 100 cm. At distances beyond 150 cm, angle and surface texture affect accuracy. The 20 cm collision threshold provides sufficient warning distance for the UGV's operational speed (~0.3 m/s at PWM 120).

### MPU6050 IMU
Raw accelerometer values include slight DC bias from module mounting. For production deployment, an offset calibration routine (recording AX/AY/AZ in level position and subtracting from all subsequent readings) is recommended. The current threshold of 15 m/s² accounts for typical mounting bias without additional calibration.

### NEO-6M GPS
Indoor GPS testing (near large windows) showed no fix. Open-sky testing at KGEC campus confirmed reliable fixes within 38–45 seconds cold start. In deployed disaster scenarios, the UGV should be given 60 seconds of outdoor GPS acquisition before entry into covered areas. Coordinates persist (valid flag) from the last fix once inside.

---

## 5. Failure Modes Encountered During Testing

| Failure | Root Cause | Detection | Resolution |
|:---|:---|:---|:---|
| ESP32-CAM camera freeze during telemetry test | GPIO 16 PSRAM clock conflict | Board restart, no stream | Isolated ESP32-CAM as standalone node, all sensors moved to Telemetry ESP32 |
| Motor jitter at joystick neutral | No deadzone applied to ADC noise | Erratic motor movement | Added ±90 unit deadzone around center (512) for car joystick |
| Flame sensor 2 false trigger on LoRa TX | GPIO 26 shared with LoRa DIO0 | Random flame alerts | Reassigned Flame Sensor 2 to GPIO 33 in firmware |
| MQ-2 reading inverted (3000+ in clean air) | Active-high comparator board variant | High ADC in clean air | Firmware auto-inversion: `gas = 4095 - raw` when baseline > 3000 |
| Controller brownout during motor start | Motor startup current spike | MCU reset during acceleration | Separated power rails: motors on raw battery, MCUs on regulated 5V |

---

*Previous: [09 — Implementation ←](./09_Implementation.md) | Next: [11 — Challenges & Solutions →](./11_Challenges_and_Solutions.md)*
