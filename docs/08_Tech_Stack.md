# 💻 08 — Technology Stack: Hardware, Firmware, Libraries & Development Environment

---

## 1. Hardware Stack

### 1.1 Microcontroller & Processing Units

| Board | Model | Clock Speed | Flash | SRAM | Role in System |
|:---|:---|:---:|:---:|:---:|:---|
| ESP32 DevKit | Espressif ESP32-WROOM-32 | 240 MHz (dual-core) | 4 MB | 520 KB | Telemetry Node (tx-lora) |
| ESP32 DevKit | Espressif ESP32-WROOM-32 | 240 MHz (dual-core) | 4 MB | 520 KB | Base Station LoRa Receiver (rx-lora) |
| ESP32-CAM | AI-Thinker (ESP32-S + OV2640) | 240 MHz | 4 MB + 8 MB PSRAM | 520 KB | Standalone video surveillance node |
| Arduino Nano | ATmega328P V3.0 | 16 MHz | 32 KB | 2 KB | Remote Controller Transmitter (tx.ino) |
| Arduino Nano | ATmega328P V3.0 | 16 MHz | 32 KB | 2 KB | Chassis Motor/Servo Controller (rx.ino) |

**ESP32-WROOM-32 Key Specs (from Espressif Datasheet):**
- Dual-core Xtensa LX6 (240 MHz), Wi-Fi 802.11 b/g/n, Bluetooth 4.2 BLE/Classic.
- 18 × 12-bit ADC channels, 3 × UART, 2 × I2C, 4 × SPI (2 usable), 16 × PWM.
- Operating voltage: 2.2V–3.6V (3.3V nominal).

**ATmega328P Key Specs (from Microchip Datasheet):**
- 8-bit AVR RISC architecture, 16 MHz with external crystal.
- 6 × 10-bit ADC channels, 1 × UART, 1 × I2C, 1 × SPI.
- 6 × PWM output channels (3 × 8-bit Timer0/2, 1 × 16-bit Timer1).

---

### 1.2 RF Transceivers

| Module | IC | Frequency | Data Rate | TX Power | RX Sensitivity | Interface |
|:---|:---|:---:|:---:|:---:|:---:|:---|
| nRF24L01+ PA+LNA | Nordic nRF24L01+ | 2.4 GHz ISM | 250 kbps / 1 Mbps / 2 Mbps | +20 dBm (PA) | −94 dBm | SPI (4-wire) |
| SX1278 LoRa RA-02 | Semtech SX1278 | 433 MHz ISM | 0.018–37.5 kbps | +20 dBm | −148 dBm | SPI (4-wire) |

**nRF24L01+ Operating Details:**
- PA+LNA variant provides +20 dBm TX power (standard module: 0 dBm).
- At 250 kbps rate, receiver sensitivity improves by ~12 dB over 2 Mbps, substantially extending range.
- Auto-ACK + auto-retransmit (up to 3 retries) with CRC-16 error checking.
- Address pipe width: 3–5 bytes. Configured to 5-byte address in firmware.

**SX1278 LoRa Operating Details:**
- Semtech LoRa proprietary spread-spectrum modulation.
- Spreading Factor: SF7–SF12 (firmware uses default SF7 for speed/range balance).
- Bandwidth: 125 kHz (default). Error Coding Rate: 4/5.
- −148 dBm sensitivity enables link budget of ~168 dB — exceptional for a $5 module.

---

### 1.3 Environmental Sensors

| Sensor | Model | Interface | Signal | Operating Range | Datasheet Key Specs |
|:---|:---|:---|:---|:---:|:---|
| Gas & Smoke | MQ-2 | Analog ADC | 0–5V / 0–4095 ADC | 300–10,000 ppm | Sensitive to LPG, CH₄, H₂, CO, Alcohol, Smoke |
| Vibration | SW-420 | Analog ADC | 0–3.3V / 0–4095 | N/A (shock) | Spring-coil vibration/tilt sensor; threshold-configurable via on-board potentiometer |
| Flame (×2) | KY-026 | Digital | Active LOW | 760–1100 nm | 5 mm photodiode + LM393 comparator; adjustable sensitivity |
| Ultrasonic | HC-SR04 | Digital (pulse) | Echo pulse width | 2–400 cm | Accuracy: ±3 mm; 40 kHz emitter; beam angle: ~15° |
| IMU | GY-521 MPU6050 | I2C (0x68) | 16-bit registers | ±2g–±16g accel | 6-axis MEMS; ±2g range (firmware default); Gyro ±250°/s |
| GPS | u-blox NEO-6M | UART 9600 baud | NMEA 0183 | Global | 50-channel GPS; 1 Hz update rate; CEP: 2.5 m; Cold start: ~30 s |

**MQ-2 Sensitivity Notes:**
The MQ-2 does not output calibrated PPM values directly. The analog output is a resistance-based voltage proportional to gas concentration. For field use, the threshold-based alert (ADC > 1800) provides reliable dangerous concentration detection without requiring a precise calibration curve. The firmware's auto-inversion logic handles both active-high and active-low comparator board configurations.

**HC-SR04 Timing Notes:**
Ultrasonic trigger: minimum 10 µs HIGH pulse. Echo pulse width in µs = 2 × distance (cm) / 0.034. At 400 cm maximum range, echo pulse = 23.5 ms. The `pulseIn()` timeout of 30,000 µs (30 ms) captures the full range without loop hang.

---

### 1.4 Actuators & Drive System

| Component | Model | Specs | Role |
|:---|:---|:---|:---|
| Motor Driver | L298N Dual H-bridge | 2A/ch continuous, 3A peak; 5V–46V motor supply | DC motor speed + direction control |
| DC Drive Motors | BO Plastic Geared (×4) | 3V–6V, ~150 RPM @ 6V, ~65 RPM @ 3V | Chassis 4-wheel drive |
| Metal Servo | MG90S (×2) | 1.8 kg·cm @ 4.8V; 0.1 s/60° speed; Metal gears | Shoulder + Elbow arm joints |
| Micro Servo | SG90 (×2) | 1.6 kg·cm @ 4.8V; 0.12 s/60° speed; Plastic gears | Gripper claw + Sensor sweep pan |

**L298N Driver Notes:**
- IN1–IN4: direction control (HIGH/LOW logic).
- ENA, ENB: PWM speed control. `analogWrite(ENA, 120)` = 47% duty cycle @ ~490 Hz PWM.
- Heat sink required for sustained full-load operation. Motor supply (VM pin) rated up to 46V, connected to 7.4V–8.4V battery in this system.

---

### 1.5 Output & Display Devices

| Component | Model | Interface | Purpose |
|:---|:---|:---|:---|
| Red LED | 5mm LED (red) | Digital GPIO 12 | Alert indicator (danger state) |
| Green LED | 5mm LED (green) | Digital GPIO 14 | Status indicator (safe state) |
| Active Buzzer | 5V active piezo | Digital GPIO 15 | Audible ~2500 Hz alarm |
| OLED Display | SSD1306 0.96" | I2C (0x3C) | Local sensor readout (shared I2C bus with MPU6050) |

---

## 2. Software & Firmware Stack

### 2.1 Development Environment

| Tool | Version | Purpose |
|:---|:---|:---|
| Arduino IDE | 2.x | Firmware coding, compilation, and board flashing |
| ESP32 Board Core | 2.x or 3.x | ESP32/ESP32-CAM Arduino board support package |
| Arduino AVR Core | 1.8.x | ATmega328P (Arduino Nano) board support |
| VS Code + PlatformIO | (Optional) | Advanced development with IntelliSense and build system |
| Python 3.x | 3.9+ | Base station serial bridge and dashboard backend scripting |
| Node.js | 18.x LTS | WebSocket relay from serial bridge to web dashboard |

---

### 2.2 Firmware Libraries

| Library | Author | Version | Purpose |
|:---|:---|:---|:---|
| `RF24` | TMRh20 / Avamander | 1.4.x | nRF24L01+ SPI transceiver driver |
| `LoRa` | Sandeep Mistry | 0.8.x | SX1278 LoRa SPI transceiver driver |
| `TinyGPSPlus` | Mikal Hart | 1.0.x | NEO-6M NMEA sentence parsing library |
| `Adafruit_MPU6050` | Adafruit Industries | 2.x | MPU6050 I2C register abstraction |
| `Adafruit_Sensor` | Adafruit Industries | 1.x | Unified Sensor API base class |
| `Wire` | Arduino Standard | Built-in | I2C bus communication |
| `SPI` | Arduino Standard | Built-in | SPI bus communication |
| `HardwareSerial` | ESP32 Core | Built-in | Hardware UART for GPS |
| `Servo` | Arduino Standard | Built-in | PWM servo control (50 Hz) |
| `ArduinoJson` | Benoit Blanchon | 6.x | JSON telemetry packaging at base receiver |
| `esp_camera.h` | Espressif | ESP-IDF | OV2640 camera driver for ESP32-CAM |
| `Adafruit_SSD1306` | Adafruit Industries | 2.x | OLED display I2C driver |
| `Adafruit_GFX` | Adafruit Industries | 1.x | OLED graphics rendering library |

---

### 2.3 Dashboard / Frontend Stack

| Technology | Tool/Framework | Version | Purpose |
|:---|:---|:---|:---|
| **Markup** | HTML5 | W3C Standard | Dashboard structure and sensor panels |
| **Styling** | CSS3 / Vanilla CSS | W3C Standard | Responsive UI layout and alert colors |
| **Logic** | JavaScript (ES6+) | Browser Native | DOM updates, WebSocket client, map rendering |
| **Map** | Leaflet.js or Google Maps JS API | 1.9.x / v3 | GPS coordinate visualization on map |
| **Serial Bridge** | Python `pyserial` | 3.x | Read LoRa receiver serial output from USB port |
| **WebSocket** | Python `websockets` or `socket.io` | 10.x / 4.x | Push telemetry from serial bridge to browser |
| **Video** | MJPEG `<img>` tag | Browser Native | Embed ESP32-CAM MJPEG stream in dashboard |

---

## 3. Connectivity Stack

```
┌─────────────────────────────────────────────────────────────────┐
│              RESCUEBOT CONNECTIVITY MATRIX                       │
├──────────────────┬──────────────────┬────────────────────────────┤
│  Link Type       │  Protocol        │  Use Case                  │
├──────────────────┼──────────────────┼────────────────────────────┤
│  nRF24L01+ RF    │ Enhanced ShockBurst│ UGV real-time control     │
│  SX1278 LoRa     │ LoRa PHY / LoRa  │ Telemetry uplink to base   │
│  Wi-Fi (802.11n) │ HTTP / MJPEG     │ Video stream to dashboard  │
│  I2C Bus         │ I2C (400 kHz)    │ MPU6050 + OLED             │
│  SPI Bus         │ SPI (up to 10MHz)│ nRF24L01+, SX1278 LoRa     │
│  UART2           │ UART 9600 baud   │ NEO-6M GPS data            │
│  USB-Serial      │ Serial 115200 baud│ LoRa RX → PC bridge       │
│  WebSocket       │ WS / Socket.IO   │ PC bridge → Dashboard      │
└──────────────────┴──────────────────┴────────────────────────────┘
```

---

## 4. Hardware Pin Summary by Board

> Full pin connection tables with component details are documented in [circuit_diagram/pin_connections.md](../circuit_diagram/pin_connections.md).

### Quick Reference

| Board | Critical Pins | Function |
|:---|:---|:---|
| ESP32 Telemetry TX | GPIO 27 (MQ-2), 34 (Vib), 25/33 (Flame), 2/4 (Ultrasonic), 21/22 (I2C), 16/17 (GPS), 5/13/32 (LoRa SPI) | All sensor inputs + LoRa TX |
| ESP32 LoRa RX | GPIO 5/14/26 (LoRa SPI), 18/19/23 (SCK/MISO/MOSI) | LoRa packet receive + USB forward |
| ESP32-CAM | GPIO 4 (Flash LED), internal 15-pin OV2640 interface | MJPEG stream server |
| Arduino Nano TX | A0-A3 (Joysticks), Pin 3 (Grip), Pin 9/10 (nRF CE/CSN) | Control transmitter |
| Arduino Nano RX | Pin 2/3/9/10 (L298N), Pin 5/6 (PWM), A2/A3/A4/A5 (Servos), Pin 7/8 (nRF CE/CSN) | Motor + arm controller |

---

*Previous: [07 — System Architecture ←](./07_System_Architecture.md) | Next: [09 — Implementation →](./09_Implementation.md)*
