# 🏗️ 07 — System Architecture: Hardware, Software & Communication Design

---

## 1. Architecture Philosophy

RescueBOT's architecture is built on three core design principles derived from embedded systems engineering practice and lessons learned during prototype integration:

1. **Subsystem Isolation:** Each major function (telemetry, drive control, video) runs on a dedicated microcontroller. This prevents resource contention, GPIO conflicts (critically: ESP32-CAM GPIO 16), and bus bandwidth collisions.

2. **Frequency-Separated Communications:** Control and data telemetry operate on independent frequency bands (2.4 GHz vs. 433 MHz) to ensure that Wi-Fi interference cannot disrupt either link simultaneously.

3. **Local Autonomous Operation:** The UGV must function safely even when the base station link fails. Local threshold evaluation, local alarms, and the control fail-safe all operate independently of the LoRa and Wi-Fi connections.

---

## 2. Full System Block Diagram

```
╔══════════════════════════════════════════════════════════════════════════════╗
║                        RESCUEBOT FULL SYSTEM TOPOLOGY                       ║
╠══════════════════════════════════════════════════════════════════════════════╣
║                                                                              ║
║  [OPERATOR REMOTE CONTROLLER]             [RESCUEBOT ROVER CHASSIS]          ║
║  ┌──────────────────────────────┐         ┌────────────────────────────────┐ ║
║  │  Arduino Nano V3 (TX)        │         │  ┌──────────────────────────┐  │ ║
║  │  ├─ Car Joystick (A2, A3)    │  2.4GHz │  │  Arduino Nano V3 (RX)   │  │ ║
║  │  ├─ Arm Joystick (A0, A1)    │ nRF Link│  │  ├─ L298N Motor Driver  │  │ ║
║  │  └─ Grip Switch (Pin 3)      ├────────►│  │  │  └─ 4× BO DC Motors  │  │ ║
║  │  nRF24L01+ PA+LNA (Pins 9,10)│  250kbps│  │  └─ Servos (A2,A3,A4,A5)│  │ ║
║  └──────────────────────────────┘         │  └──────────────────────────┘  │ ║
║                                           │                                │ ║
║  [BASE OPERATIONS STATION]                │  ┌──────────────────────────┐  │ ║
║  ┌──────────────────────────────┐  433MHz │  │  ESP32 Telemetry Node    │  │ ║
║  │  Web Dashboard + PC          │  LoRa   │  │  ├─ MQ-2 Gas  (GPIO 27) │  │ ║
║  │  ┌──────────────────────┐    │         │  │  ├─ SW-420 Vib(GPIO 34) │  │ ║
║  │  │  ESP32 LoRa Receiver │◄───┼─────────┤  │  ├─ KY-026×2  (25, 33) │  │ ║
║  │  │  SX1278 (5,14,26)    │    │         │  │  ├─ HC-SR04   (2, 4)   │  │ ║
║  │  └──────────────────────┘    │         │  │  ├─ MPU6050   (21, 22) │  │ ║
║  │  USB-Serial → Dashboard      │         │  │  ├─ NEO-6M GPS(16, 17) │  │ ║
║  └──────────────────────────────┘         │  │  ├─ Buzzer     (GPIO 15)│  │ ║
║                                           │  │  ├─ Red LED    (GPIO 12)│  │ ║
║  [DASHBOARD BROWSER / PC]                 │  │  └─ Green LED  (GPIO 14)│  │ ║
║  ┌──────────────────────────────┐         │  │  SX1278 LoRa (5,13,32) │  │ ║
║  │  Telemetry Gauges            │ Wi-Fi   │  └──────────────────────────┘  │ ║
║  │  GPS Map (lat/lon)           │  MJPEG  │                                │ ║
║  │  Alert Status Panels         │◄────────┤  ┌──────────────────────────┐  │ ║
║  │  Live Camera Feed [Port 81]  │         │  │  ESP32-CAM (AI-Thinker)  │  │ ║
║  └──────────────────────────────┘         │  │  OV2640 Camera Sensor    │  │ ║
║                                           │  │  Flash LED (GPIO 4 PWM)  │  │ ║
║                                           │  │  HTTP Port 81 /stream    │  │ ║
║                                           │  └──────────────────────────┘  │ ║
║                                           └────────────────────────────────┘ ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

---

## 3. Hardware Architecture: Layer-by-Layer Breakdown

### Layer 1 — Sensor Input Layer

All six environmental sensors are physically mounted on the rover chassis and electrically connected to the Telemetry ESP32:

| Sensor | GPIO | Interface | Signal Type | Polling Method |
|:---|:---:|:---|:---|:---|
| MQ-2 Gas Sensor | GPIO 27 | Analog ADC | 0–3.3V / 0–4095 | `analogRead()` each loop |
| SW-420 Vibration | GPIO 34 | Analog ADC | 0–3.3V / 0–4095 | `analogRead()` each loop |
| KY-026 Flame #1 | GPIO 25 | Digital | Active LOW | `digitalRead()` each loop |
| KY-026 Flame #2 | GPIO 33 | Digital | Active LOW | `digitalRead()` each loop |
| HC-SR04 TRIG | GPIO 2 | Digital OUT | 10 µs pulse | Output each loop |
| HC-SR04 ECHO | GPIO 4 | Digital IN | Echo duration | `pulseIn()` 30 ms timeout |
| MPU6050 SDA | GPIO 21 | I2C | Bidirectional | `Wire.begin()` / library |
| MPU6050 SCL | GPIO 22 | I2C | Clock | `Wire.begin()` / library |
| NEO-6M GPS RX | GPIO 16 | UART2 IN | NMEA serial | `HardwareSerial(2)` 9600 baud |
| NEO-6M GPS TX | GPIO 17 | UART2 OUT | NMEA serial | `HardwareSerial(2)` 9600 baud |

> **GPIO 34 is input-only** on the ESP32 (no internal pull-up). External pull-down or float may affect readings. SW-420 module has its own pull-up on the PCB.

### Layer 2 — Processing Layer

Three independent processing nodes handle distinct responsibilities:

#### 2A. Telemetry Node — ESP32-WROOM-32 (tx-lora.ino)
- **Core:** Dual-core Xtensa LX6, 240 MHz. Telemetry loop runs on Core 0.
- **Responsibilities:** Sensor polling, threshold evaluation, local alert control, LoRa packet serialization and transmission.
- **Libraries:** `Wire.h`, `SPI.h`, `LoRa.h`, `TinyGPSPlus.h`, `HardwareSerial.h`, `Adafruit_MPU6050.h`.
- **Memory:** ~256 KB SRAM (adequate for ASCII packet generation and sensor objects).

#### 2B. Motor & Arm Actuation Node — Arduino Nano V3 (rx.ino)
- **Core:** ATmega328P, 16 MHz.
- **Responsibilities:** nRF24L01+ packet reception, L298N PWM motor control, 4× servo angle management, fail-safe timeout.
- **Libraries:** `RF24.h`, `Servo.h`.
- **Timing:** Non-blocking servo sweep on 15 ms interval. Fail-safe on 500 ms timeout.

#### 2C. Remote Controller Node — Arduino Nano V3 (tx.ino)
- **Core:** ATmega328P, 16 MHz.
- **Responsibilities:** Joystick ADC reading, IIR smoothing filter, deadzone enforcement, packet encoding, nRF24L01+ transmission.
- **Libraries:** `RF24.h`.

### Layer 3 — Communication Layer

```
┌──────────────────────────────────────────────────────────────────┐
│  COMMUNICATION LAYER                                              │
├───────────────┬───────────────────────────────────────────────────┤
│  Link         │  Control Link         │  Telemetry Link           │
├───────────────┼───────────────────────┼───────────────────────────┤
│  Hardware     │  nRF24L01+ PA+LNA     │  SX1278 LoRa RA-02        │
│  Protocol     │  Proprietary RF (nRF) │  LoRa PHY + LoRa MAC      │
│  Frequency    │  2.4 GHz, Channel 108 │  433 MHz ISM Band         │
│  Data Rate    │  250 kbps             │  ~0.24–0.98 kbps (default)│
│  Range        │  ~200 m LOS           │  ~1.2 km LOS, 350 m urban │
│  Payload      │  9 bytes (DataPacket) │  ~100 bytes ASCII string  │
│  Interference │  Wi-Fi Ch. 1–13 clear │  Immune to 2.4 GHz noise  │
│  SPI Pins     │  TX: 9(CE),10(CSN)    │  5(SS),13(RST),32(DIO0)   │
│  (TX Node)    │  RX: 7(CE), 8(CSN)   │  18(SCK),19(MISO),23(MOSI)│
└───────────────┴───────────────────────┴───────────────────────────┘
```

**Why 433 MHz for telemetry?**
The 433 MHz ISM band has significantly better obstacle penetration than 2.4 GHz due to its longer wavelength (~69 cm vs. ~12.5 cm). In disaster environments (collapsed concrete structures, underground tunnels), 433 MHz signals exhibit lower free-space path loss and better diffraction around rubble. Research by Erturk et al. (IEEE Access, 2020) confirms sub-GHz LoRa achieves −119 dBm receiver sensitivity, enabling communication through multiple reinforced concrete slabs.

### Layer 4 — Actuation Layer

#### Drive System
```
Arduino Nano RX
  ├─ Pin 2 → L298N IN1 ─┐
  ├─ Pin 3 → L298N IN2 ─┤ Left motor pair direction
  ├─ Pin 9 → L298N IN3 ─┤ Right motor pair direction
  ├─ Pin 10→ L298N IN4 ─┘
  ├─ Pin 5 (PWM) → L298N ENA → Left motor speed
  └─ Pin 6 (PWM) → L298N ENB → Right motor speed

L298N Outputs:
  ├─ OUT1 + OUT2 → Motor A (Left-Front + Left-Rear)
  └─ OUT3 + OUT4 → Motor B (Right-Front + Right-Rear)
```

L298N motor driver: dual H-bridge, rated up to 2A continuous per channel (peak 3A), with built-in kickback protection diodes. Motor driver VCC (12V input pin) connected to raw battery (7.4V–8.4V). Logic supply VSS connected to regulated 5V rail.

#### Robotic Arm
```
Arduino Nano RX
  ├─ A2 (PWM) → Shoulder Servo (MG90S) → 0°–120°
  ├─ A5 (PWM) → Elbow Servo (MG90S)    → 0°–120°
  ├─ A4 (PWM) → Gripper Servo (SG90)   → 0° (Open) / 90° (Closed)
  └─ A3 (PWM) → Sweep Servo (SG90)     → 0°–180° (auto-pan)
```

Servo PWM: 50 Hz control signal, pulse width 500 µs (0°) to 2400 µs (180°).

### Layer 5 — Surveillance & Interface Layer

```
ESP32-CAM (AI-Thinker)
  ├─ OV2640 Camera (DVP parallel interface — 15 internal GPIO pins)
  ├─ GPIO 4 → Flash LED (LEDC PWM dimming)
  ├─ Wi-Fi Station Mode → SSID/PSK configured in firmware
  ├─ HTTP Server on Port 81
  └─ /stream endpoint → multipart/x-mixed-replace MJPEG
```

The dashboard browser opens `http://<cam_ip>:81/stream` as an `<img>` tag source. The browser's MJPEG decoder renders continuous frames as they arrive from the HTTP multipart boundary stream.

---

## 4. Software Architecture

### 4.1 Firmware Modules

```
firmware/
├── lora_module/
│   ├── tx-lora.ino       ← Telemetry ESP32: sensor polling + LoRa TX
│   └── rx-lora.ino       ← Base Station ESP32: LoRa RX + serial bridge
├── nrf_communication/
│   ├── tx.ino            ← Remote Controller Nano: joystick + nRF TX
│   └── rx.ino            ← Chassis Nano: nRF RX + motor/servo control
└── cam_module/
    └── production.ino    ← ESP32-CAM: MJPEG stream server
```

### 4.2 Software Data Flow

```
Firmware Layers:
┌───────────────────────────────────────────────────────┐
│  Application Layer      │ Threshold logic, alert eval  │
│                         │ Servo angle management       │
│                         │ Motor direction mapping      │
├───────────────────────────────────────────────────────┤
│  Middleware / Library   │ Adafruit_MPU6050, TinyGPSPlus│
│  Layer                  │ RF24, LoRa, ArduinoJson      │
│                         │ Servo, HardwareSerial        │
├───────────────────────────────────────────────────────┤
│  HAL / Driver Layer     │ Wire (I2C), SPI bus          │
│                         │ analogRead, digitalRead      │
│                         │ LEDC PWM, UART               │
├───────────────────────────────────────────────────────┤
│  Hardware Layer         │ ESP32, Arduino ATmega328P    │
│                         │ nRF24L01+, SX1278 LoRa       │
│                         │ MPU6050, NEO-6M, MQ-2, etc.  │
└───────────────────────────────────────────────────────┘
```

### 4.3 Non-Blocking Execution Pattern

All timing-critical firmware uses `millis()` guards:

```cpp
unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL = 1000;

void loop() {
    if (millis() - lastSendTime >= SEND_INTERVAL) {
        lastSendTime = millis();
        // Execute sensor read + LoRa transmit
    }
    // All other loop code runs continuously (GPS, RF polling)
}
```

This ensures GPS character ingestion (`gps.encode()`) and nRF24L01+ receive checking occur at maximum frequency without being blocked by the 1000 ms sensor/LoRa cycle.

---

## 5. Power Architecture

```
[2S Li-ion 18650 Battery Pack]
       │  7.4V–8.4V raw
       │
       ├──► [L298N Motor Driver VCC]
       │         └──► 4× BO DC Geared Motors
       │
       ├──► [Buck Converter 5V @ 2A]
       │         ├──► Arduino Nano RX (5V pin)
       │         ├──► Arduino Nano TX (5V pin)
       │         ├──► MG90S + SG90 Servos (shared 5V rail)
       │         ├──► HC-SR04 Ultrasonic VCC
       │         └──► Active Buzzer VCC
       │
       ├──► [On-board 3.3V regulators (ESP32)]
       │         ├──► ESP32 Telemetry Node (3.3V LDO)
       │         ├──► MPU6050 VCC (3.3V via ESP32 3V3 pin)
       │         ├──► NEO-6M GPS VCC (3.3V)
       │         └──► SX1278 LoRa VCC (3.3V)
       │
       └──► [External 5V USB supply for ESP32-CAM]
                 └──► ESP32-CAM VCC (5V required — USB or regulator)
```

> **Critical:** ESP32-CAM requires 5V input (not 3.3V). The AI-Thinker board has an onboard AMS1117-3.3 LDO that converts 5V → 3.3V for the ESP32 module. Supplying the CAM directly from a 3.3V rail will prevent boot.

---

## 6. PCB & Wiring Strategy

Given the prototype nature of the build, the circuit is assembled on:
- **Breadboards** for sensor connections on the Telemetry ESP32.
- **Prototype PCBs** (veroboard/perfboard) for permanent motor driver and servo connections on the Arduino Nano RX.
- **Dupont jumper wires** for all inter-board connections.
- **Decoupling capacitors** (100 nF ceramic, 10 µF electrolytic) at MQ-2 and SW-420 power pins to suppress switching noise from motor currents.

---

*Previous: [06 — Working Principle ←](./06_Working_Principle.md) | Next: [08 — Tech Stack →](./08_Tech_Stack.md)*
