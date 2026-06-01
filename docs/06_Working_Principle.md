# 🔄 06 — Working Principle: Data Flow, Operational Logic & System Cycles

---

## Overview

RescueBOT operates as three concurrent, loosely coupled control loops running simultaneously across five microcontroller boards. Understanding the timing and data flow of each loop is essential for comprehending how the platform achieves real-time response to hazardous conditions while maintaining stable remote control and independent video surveillance.

---

## 1. Full System Operational Flowchart

```
═══════════════════════════════════════════════════════════════════
                    RESCUEBOT OPERATIONAL LOOP
═══════════════════════════════════════════════════════════════════

LOOP A: TELEMETRY CYCLE (ESP32 Telemetry Node — tx-lora.ino)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  [START LOOP — every 1000 ms]
       │
       ├─► [Poll MQ-2 Gas Sensor — analogRead(GPIO 27)]
       │         └─ Apply calibration: if raw > 3000 → GAS = 4095 - raw
       │
       ├─► [Poll SW-420 Vibration — analogRead(GPIO 34)]
       │         └─ Map to 0–100% intensity scale
       │
       ├─► [Poll KY-026 Flame × 2 — digitalRead(GPIO 25, 33)]
       │         └─ Detect: flame = (FL1==LOW || FL2==LOW)
       │
       ├─► [Poll HC-SR04 Ultrasonic]
       │         ├─ Send 10µs pulse on GPIO 2 (TRIG)
       │         ├─ pulseIn(GPIO 4, HIGH, 30000µs timeout)
       │         └─ DIST = (duration × 0.034) / 2.0  [cm]
       │
       ├─► [Poll MPU6050 via I2C (GPIO 21/22)]
       │         └─ Read AX, AY, AZ accelerometer vectors (m/s²)
       │
       ├─► [Update GPS via UART2 (GPIO 16)]
       │         └─ gps.encode(gpsSerial.read()) → parse LAT/LON
       │
       ├─► [EVALUATE ALERT CONDITIONS]
       │         ├─ GAS > 1800?     → alert = true
       │         ├─ VIB > 1500?     → alert = true
       │         ├─ FLAME == LOW?   → alert = true
       │         ├─ DIST < 20 cm?   → alert = true
       │         └─ |AX|>15||AY|>15?→ alert = true
       │
       ├─► [TRIGGER LOCAL ALERT HARDWARE]
       │         ├─ alert=true:  RED_LED(12)=HIGH, BUZZER(15)=HIGH, GRN(14)=LOW
       │         └─ alert=false: RED_LED(12)=LOW,  BUZZER(15)=LOW,  GRN(14)=HIGH
       │
       ├─► [SERIALIZE DATA PACKET]
       │         └─ "VIB=xx,GAS=xx,FL1=x,FL2=x,DIST=xx.xx,
       │               AX=x.xx,AY=x.xx,AZ=x.xx,LAT=xx.xxxxxx,
       │               LON=xx.xxxxxx,ALERT=x"
       │
       └─► [TRANSMIT VIA LORA SX1278 — 433 MHz]
                 └─ LoRa.beginPacket() → LoRa.print(data) → LoRa.endPacket()

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
LOOP B: CONTROL CYCLE (Arduino Nano TX → RX — tx.ino / rx.ino)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  [TX: Continuous at ~20 ms intervals]
       │
       ├─► Read Car Joystick: A2=VRX, A3=VRY (0–1023)
       ├─► Read Arm Joystick: A0=VRX, A1=VRY (0–1023)
       ├─► Read Grip Switch: Pin 3 (INPUT_PULLUP toggle)
       │
       ├─► Apply Smoothing:
       │         smooth = (raw - smooth) × 0.35 + smooth
       │
       ├─► Apply Deadzones:
       │         Car: |val - 512| < 90 → set to 0
       │         Arm: |val - 512| < 70 → set to 0
       │
       ├─► Build 9-byte DataPacket: {carX, carY, armX, armY, carMove, armMove, grip}
       └─► nRF24L01+ TX → 2.4 GHz Channel 108 → 250 kbps

  [RX: Continuous, non-blocking poll]
       │
       ├─► radio.available()? → Read 9-byte packet
       │         │
       │         ├─► Map carX/carY → L298N motor directions (IN1-IN4)
       │         │         ├─ Forward: IN1=1,IN2=0,IN3=0,IN4=1 @ PWM 120
       │         │         ├─ Backward:IN1=0,IN2=1,IN3=1,IN4=0 @ PWM 120
       │         │         ├─ Left:    IN1=0,IN2=1,IN3=0,IN4=1 @ PWM 100
       │         │         └─ Right:   IN1=1,IN2=0,IN3=1,IN4=0 @ PWM 100
       │         │
       │         ├─► Map armX/armY → servo joint angle increments ±5°
       │         │         ├─ Shoulder (A2): 0°–120° range
       │         │         ├─ Elbow (A5):   0°–120° range
       │         │         └─ Grip (A4):    0° or 90° (toggle)
       │         │
       │         └─► Auto-Pan: Sweep servo (A3) → 0°↔180°, +1° every 15 ms
       │
       └─► Fail-Safe: if (millis() - lastPacketTime > 500):
                 └─ ENA=0, ENB=0 → MOTORS HALT

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
LOOP C: VIDEO CYCLE (ESP32-CAM — production.ino)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  [Concurrent HTTP server — Port 81]
       │
       ├─► OV2640 captures frame (VGA 640×480 with PSRAM)
       ├─► Frame encoded as MJPEG
       ├─► HTTP multipart boundary appended
       ├─► Pushed to connected HTTP client (Dashboard browser)
       └─► Flash LED (GPIO 4) PWM brightness adjustable
═══════════════════════════════════════════════════════════════════
```

---

## 2. Step-by-Step Technical Workflow

### Step 1 — Power-On Initialization

When power is applied (2S Li-ion, 7.4V–8.4V):

1. **All ESP32 boards** boot and initialize serial output at 115200 baud.
2. The **Telemetry ESP32** initializes the I2C bus (GPIO 21/22), begins MPU6050 connection, starts UART2 for GPS at 9600 baud, and initializes the SX1278 LoRa module at 433 MHz.
3. The **Arduino Nano RX** initializes the nRF24L01+ in receive mode on Channel 108 and sets all motor/servo pins to their safe default states (motors stopped, arm servos at centered positions).
4. The **Arduino Nano TX** initializes the nRF24L01+ in transmit mode and begins reading joystick ADC values.
5. The **ESP32-CAM** connects to the configured Wi-Fi SSID and starts the HTTP streaming server on Port 81.

### Step 2 — Sensor Data Acquisition (Telemetry Loop)

The ESP32 Telemetry Node enters its main loop immediately after initialization. The sensor acquisition sequence within each 1000 ms cycle is:

**Gas Sensor Reading:**
The MQ-2's analog output is read via 12-bit ADC (0–4095 range at 0–3.3V VCC). The sensor's internal comparator produces a voltage proportional to gas concentration. The ESP32 ADC is non-linear above ~3.0V, so absolute PPM calibration requires a lookup curve; however, the threshold-based alert (>1800) is reliable for detecting dangerous concentrations of LPG, methane, and CO — the primary disaster gas hazards.

**Ultrasonic Distance Measurement:**
```
TRIG (GPIO 2) → 10 µs HIGH pulse
  ↓
HC-SR04 emits 8-cycle 40 kHz ultrasonic burst
  ↓
Burst reflects off obstacle surface
  ↓
ECHO (GPIO 4) goes HIGH for duration = 2 × distance / speed_of_sound
  ↓
pulseIn(ECHO_PIN, HIGH, 30000) → duration in µs
  ↓
Distance = duration × 0.034 / 2.0 [cm]
```
Speed of sound constant: 0.034 cm/µs (343 m/s at 20°C ambient).

**MPU6050 IMU Reading:**
The Adafruit MPU6050 library communicates over I2C. `mpu.getEvent(&a, &g, &temp)` returns acceleration in m/s². In level position, AZ ≈ +9.81 m/s² (gravity vector). As the rover tilts, gravity redistributes across AX and AY:
- A 30° tilt produces AX ≈ 4.9 m/s²
- A 57° tilt produces AX ≈ 8.2 m/s²
- The 15 m/s² threshold catches extreme tilts beyond ~57° — suitable for tip-over detection.

**GPS Data:**
The TinyGPSPlus library parses NMEA 0183 sentences (specifically $GPRMC and $GPGGA) from the NEO-6M. Every byte received from UART2 is fed through `gps.encode()`. When a complete, valid sentence is parsed, `gps.location.isValid()` returns true and coordinates are updated.

### Step 3 — Threshold Evaluation & Alert Generation

The alert evaluation is a single boolean OR chain:
```cpp
bool alert = (gasValue > gasThreshold)     // 1800
          || (vibrationValue > vibThreshold) // 1500
          || (flame1 == LOW)
          || (flame2 == LOW)
          || (distance > 0 && distance < distThreshold) // 20 cm
          || (abs(ax) > 15 || abs(ay) > 15);
```

This `alert` boolean simultaneously:
1. Drives the local Red LED and Buzzer (immediate physical response).
2. Sets the `ALERT=1` field in the LoRa telemetry packet (remote dashboard notification).

### Step 4 — LoRa Packet Transmission

All sensor values are concatenated into a single ASCII string:
```
"VIB=12,GAS=412,FL1=1,FL2=1,DIST=85.00,AX=0.12,AY=-0.08,AZ=9.79,LAT=22.952311,LON=88.473862,ALERT=0"
```

This format is chosen for:
- **Human readability** during serial debug monitoring.
- **Simple parsing** at the base station receiver (split on commas, then on `=`).
- **Compact size** — typically 80–110 ASCII characters, well within LoRa's maximum payload.

`LoRa.beginPacket()` → `LoRa.print(data)` → `LoRa.endPacket()` — the SX1278 handles FHSS, error correction, and CRC internally at the LoRa physical layer.

### Step 5 — Base Station Reception & Dashboard Relay

The base station ESP32 (running `rx-lora.ino`) receives the LoRa packet:
```cpp
while (LoRa.available()) {
    receivedData += (char)LoRa.read();
}
Serial.println(receivedData); // Forward to USB-Serial → PC
```

A Node.js or Python script reads the USB-Serial port, parses the key-value string, and pushes the structured data to the web dashboard via WebSocket or HTTP polling. The dashboard renders sensor gauges, map pins, alert indicators, and the embedded MJPEG camera feed.

### Step 6 — Remote Control Execution

Concurrently with the telemetry loop, the nRF24L01+ link runs independently at ~50 Hz:

1. **Operator** moves joystick → Arduino Nano TX reads analog ADC → applies smoothing + deadzones → packs `DataPacket` struct → transmits via nRF24L01+ at 250 kbps.
2. **Arduino Nano RX** receives packet → maps `carX/carY` to L298N direction pins → maps `armX/armY` to servo angle increments → auto-increments the sweep servo by ±1° every 15 ms.
3. **Fail-safe** monitors time since last received packet. Exceeding 500 ms → `ENA = ENB = 0` → motors halt.

---

## 3. Data Flow Diagram: Sensor to Dashboard

```
[PHYSICAL ENVIRONMENT]
         │
         │ Analog/Digital Signals
         ▼
[TELEMETRY ESP32 NODE]─────────────► [Local Alert Hardware]
  • MQ-2, SW-420, KY-026×2           • Red LED, Buzzer, Green LED
  • HC-SR04, MPU6050, NEO-6M
         │
         │ Serialized ASCII string (433 MHz LoRa)
         ▼
[BASE STATION ESP32 RECEIVER]
         │
         │ USB-Serial bridge
         ▼
[NODE.JS / PYTHON RELAY SCRIPT]
         │
         │ WebSocket / HTTP push
         ▼
[WEB DASHBOARD BROWSER]
  • Sensor gauge panels
  • GPS map overlay (lat/lon)        [ESP32-CAM Wi-Fi Stream]
  • Alert status indicators    ◄─────• MJPEG Port 81
  • Telemetry log history            • Embedded in dashboard
```

---

## 4. Timing Summary

| Loop | Board | Cycle Time | Protocol |
|:---|:---|:---|:---|
| Telemetry & Alert | ESP32 Telemetry | ~1000 ms | LoRa 433 MHz |
| RF Control | Arduino Nano TX/RX | ~20 ms | nRF24L01+ 2.4 GHz |
| Video Stream | ESP32-CAM | ~45 ms per frame | Wi-Fi MJPEG |
| Fail-Safe Watchdog | Arduino Nano RX | 500 ms timeout | N/A |
| Sweep Servo Auto-Pan | Arduino Nano RX | 15 ms per 1° step | PWM |

---

*Previous: [05 — Innovation & USP ←](./05_Innovation_and_USP.md) | Next: [07 — System Architecture →](./07_System_Architecture.md)*
