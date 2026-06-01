# 🔧 09 — Implementation: Build Process, Circuit Integration & Firmware Interfacing

---

## 1. Physical Build & Mechanical Assembly

### 1.1 Chassis Construction

The RescueBOT UGV is built on a **two-layer acrylic car chassis** (standard 4WD car kit, approximately 220mm × 170mm × 70mm assembled height):

**Bottom Layer:**
- Four BO plastic geared DC motors are mounted to the lower chassis plate using M3 bolts — two on each lateral side, forming two motor pairs for differential drive.
- Each motor pair (left/right) shares a common power supply from the L298N output channels.
- The **L298N motor driver** is mounted centrally on the lower plate between the motor pairs, minimizing wire length and improving heat dissipation from the aluminum heat sink.
- The **2S Li-ion 18650 battery pack** is mounted in the center for weight balance. A rocker switch in the battery line enables safe power disconnection without unplugging connections.

**Upper Layer:**
- The **Arduino Nano RX** (motor + servo controller) is mounted on the upper plate, connected to the L298N via short jumper wires.
- The **Telemetry ESP32** is mounted near the front-center of the upper layer for clear sensor wire routing.
- The **SX1278 LoRa module** is mounted adjacent to the ESP32, connected via SPI bus wires.
- A **5V buck converter** is mounted on the upper layer, sourcing from the raw battery to provide regulated 5V for Arduino Nano, servos, HC-SR04, and buzzer.

**Front Sensor Bar:**
- The **HC-SR04 ultrasonic sensor** is mounted on the SG90 sweep servo bracket at the front of the upper chassis, pointing forward. The sweep servo pans the ultrasonic sensor side-to-side for a full hemisphere scan.
- **KY-026 Flame Sensors (×2)** are mounted symmetrically on either side of the front bar, angled slightly outward for wider IR detection coverage.

**Sensor Placement:**
- **MQ-2 Gas Sensor:** Mounted on the upper chassis deck, positioned near the UGV's air intake path. Kept away from motor heat sources to reduce thermal interference.
- **SW-420 Vibration Sensor:** Mounted directly on the chassis base layer, making direct contact with the chassis frame to pick up structural vibrations transferred through wheels and suspension.
- **MPU6050 IMU:** Mounted flat and level on the upper layer. Orientation matters — the firmware reads raw acceleration vectors, so a level mounting ensures the gravity vector is correctly distributed across the measured axes.
- **NEO-6M GPS Module:** Mounted on the topmost position (or externally on a small mast) with the ceramic patch antenna facing skyward for maximum satellite visibility.

**Robotic Arm:**
- The 4DOF arm assembly is mounted at the front of the upper chassis.
- The Shoulder MG90S servo is base-fixed to the chassis, enabling the entire arm to rotate vertically.
- The Elbow MG90S servo is attached to the shoulder output shaft, bending the forearm.
- The Gripper SG90 servo is at the terminal end, opening and closing the claw.
- Wire routing follows the joint structure to minimize wire tension during arm movement.

**ESP32-CAM:**
- Mounted in a 3D-printed or bracket housing at the front of the upper layer, oriented forward. The camera housing protects the lens from debris while maintaining an unobstructed forward viewing angle.

---

## 2. Electrical Circuit Integration

### 2.1 Power Architecture Implementation

The UGV uses a **three-rail power distribution** scheme to protect microcontrollers from motor-induced voltage transients:

```
Battery Pack (2S Li-ion, 7.4V–8.4V)
│
├──[Rail 1: Raw Battery → L298N VM pin]
│      └─► 4× BO DC Geared Motors (direct high-current drive)
│
├──[Rail 2: Buck Converter → 5V regulated]
│      ├─► Arduino Nano RX (5V pin)
│      ├─► Arduino Nano TX (5V pin)
│      ├─► MG90S × 2 + SG90 × 2 Servos (5V power)
│      ├─► HC-SR04 VCC
│      ├─► Active Buzzer VCC
│      └─► L298N logic VSS (5V logic supply)
│
└──[Rail 3: ESP32 onboard LDO → 3.3V regulated]
       ├─► MPU6050 VCC (via ESP32 3V3 pin)
       ├─► NEO-6M GPS VCC
       ├─► SX1278 LoRa VCC
       ├─► KY-026 Flame Sensors VCC
       └─► SW-420 Vibration Sensor VCC
```

**Common Ground:** All grounds (battery negative, regulator output GND, Arduino GND, ESP32 GND, L298N GND, motor return, servo signal return) are tied to a single ground bus point on the prototype breadboard/veroboard.

**Decoupling:**
- 100 nF ceramic capacitors placed at MQ-2 VCC and SW-420 VCC pins to filter high-frequency switching noise from motor PWM.
- 10 µF electrolytic capacitor at the buck converter output to stabilize the 5V rail during servo stall currents.

### 2.2 I2C Bus Wiring

The MPU6050 and SSD1306 OLED display share the I2C bus:

```
ESP32 GPIO 21 (SDA) ──[4.7kΩ pull-up to 3.3V]──┬── MPU6050 SDA
                                                  └── SSD1306 SDA (addr 0x3C)

ESP32 GPIO 22 (SCL) ──[4.7kΩ pull-up to 3.3V]──┬── MPU6050 SCL
                                                  └── SSD1306 SCL
```

Pull-up resistors are required for I2C signal integrity. Without them, the open-drain I2C lines rise too slowly, causing communication errors at speeds above ~100 kHz. The firmware uses the default Arduino Wire library speed (100 kHz), well within both devices' specifications.

### 2.3 SPI Bus Wiring (LoRa)

The SX1278 LoRa module shares the ESP32's hardware SPI bus:

```
ESP32 GPIO 18 (SCK)  → LoRa SCK
ESP32 GPIO 19 (MISO) → LoRa MISO
ESP32 GPIO 23 (MOSI) → LoRa MOSI
ESP32 GPIO 5  (NSS)  → LoRa CS (Chip Select, active LOW)
ESP32 GPIO 13 (RST)  → LoRa RST (Reset, active LOW)
ESP32 GPIO 32 (DIO0) → LoRa DIO0 (TX/RX Done interrupt)
```

> The ESP32's hardware SPI (VSPI) is also used internally for Flash memory access. The LoRa library handles CS pin management correctly, ensuring no bus contention.

### 2.4 nRF24L01+ Wiring (Arduino Nano TX)

```
Arduino Nano Pin 9  (CE)  → nRF24L01+ CE
Arduino Nano Pin 10 (CSN) → nRF24L01+ CSN
Arduino Nano Pin 11 (MOSI)→ nRF24L01+ MOSI
Arduino Nano Pin 12 (MISO)→ nRF24L01+ MISO
Arduino Nano Pin 13 (SCK) → nRF24L01+ SCK
Arduino Nano 3.3V         → nRF24L01+ VCC (3.3V — CRITICAL)
Arduino Nano GND          → nRF24L01+ GND
```

**Critical:** The nRF24L01+ operates at 3.3V logic and VCC. Applying 5V to the VCC pin will permanently damage the module. The Arduino Nano's onboard AMS1117-3.3 regulator provides the necessary 3.3V from its dedicated output pin.

---

## 3. Firmware Implementation Details

### 3.1 Flashing Procedure

**For ESP32 boards:**
1. Install ESP32 board core in Arduino IDE: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. Select board: `ESP32 Dev Module` (for DevKit) or `AI Thinker ESP32-CAM` (for camera module).
3. Set Upload Speed: 921600 baud. Set CPU Frequency: 240 MHz.
4. For ESP32-CAM: Use USB-TTL adapter (e.g., CH340G) connected to U0TXD/U0RXD pins with GPIO0 pulled to GND during flash, then released for normal operation.

**For Arduino Nano:**
1. Select board: `Arduino Nano`. Set Processor: `ATmega328P (Old Bootloader)` if using clone Nanos with CH340G USB chip.
2. Upload Speed: 115200 baud.

### 3.2 Key Firmware Logic Implementations

#### Telemetry Node (tx-lora.ino) — Critical Code Patterns

**MQ-2 Auto-Calibration:**
```cpp
int gasValue = analogRead(MQ2_PIN);
// Active-HIGH board variant detection and correction
// (High baseline reading in clean air = inverted board)
// if (gasValue > 3000 in clean-air baseline) → gasValue = 4095 - gasValue;
```

**Ultrasonic Non-Blocking Measurement:**
```cpp
digitalWrite(TRIG_PIN, LOW);
delayMicroseconds(2);
digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);

long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms max timeout
float distance = (duration > 0) ? (duration * 0.034 / 2.0) : 999.0;
```

**LoRa Packet Assembly:**
```cpp
String data =
    "VIB=" + String(vibrationValue) +
    ",GAS=" + String(gasValue) +
    ",FL1=" + String(flame1) +
    ",FL2=" + String(flame2) +
    ",DIST=" + String(distance, 2) +
    ",AX=" + String(ax, 2) +
    ",AY=" + String(ay, 2) +
    ",AZ=" + String(az, 2) +
    ",LAT=" + String(latitude, 6) +
    ",LON=" + String(longitude, 6) +
    ",ALERT=" + String(alert);

LoRa.beginPacket();
LoRa.print(data);
LoRa.endPacket();
```

#### Controller Node (tx.ino) — Joystick Smoothing

```cpp
// Exponential Moving Average filter
// α = 0.35 (35% weight to current reading, 65% to historical)
smooth = (raw - smooth) * 0.35 + smooth;

// Deadzone enforcement
if (abs(smooth - 512) < DEADZONE_CAR) smooth = 512; // Dead center
```

#### Receiver Node (rx.ino) — Fail-Safe Implementation

```cpp
unsigned long lastReceiveTime = 0;
const unsigned long TIMEOUT_MS = 500;

void loop() {
    if (radio.available()) {
        radio.read(&data, sizeof(data));
        lastReceiveTime = millis();
        // Apply motor and servo commands
    }

    // Fail-safe: halt if no packet within 500 ms
    if (millis() - lastReceiveTime > TIMEOUT_MS) {
        analogWrite(ENA, 0);
        analogWrite(ENB, 0);
        // All motor pins LOW
    }
}
```

### 3.3 ESP32-CAM Firmware (production.ino)

The camera firmware handles two ESP32 Arduino Core API differences:

```cpp
#if defined(ARDUINO_ESP32_RELEASE_3)
    // v3.x API
    ledcAttach(LED_PIN, 5000, 8);
    ledcWrite(LED_PIN, brightness);
#else
    // v2.x API
    ledcSetup(0, 5000, 8);
    ledcAttachPin(LED_PIN, 0);
    ledcWrite(0, brightness);
#endif
```

This ensures the firmware compiles and runs correctly on both major ESP32 Arduino Core versions without manual modification.

**Camera Config (with PSRAM):**
```cpp
config.frame_size = FRAMESIZE_VGA;   // 640×480
config.fb_count = 2;                  // Double buffer for streaming
config.jpeg_quality = 10;             // JPEG compression (0=best, 63=worst)
```

---

## 4. Dashboard Implementation

### 4.1 Serial Bridge Script (Python)

```python
import serial
import websockets
import asyncio
import json

ser = serial.Serial('/dev/ttyUSB0', 115200)  # LoRa RX board

async def relay():
    async with websockets.connect('ws://localhost:8080') as ws:
        while True:
            line = ser.readline().decode('utf-8').strip()
            # Parse: "VIB=12,GAS=412,FL1=1,..."
            fields = dict(item.split('=') for item in line.split(','))
            await ws.send(json.dumps(fields))

asyncio.run(relay())
```

### 4.2 Dashboard Frontend (JavaScript)

```javascript
const ws = new WebSocket('ws://localhost:8080');

ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    
    // Update sensor panels
    document.getElementById('gas-value').textContent = data.GAS;
    document.getElementById('vib-value').textContent = data.VIB;
    document.getElementById('dist-value').textContent = data.DIST;
    
    // Update alert status
    const alertBanner = document.getElementById('alert-banner');
    alertBanner.className = data.ALERT === '1' ? 'danger' : 'safe';
    
    // Update GPS map pin
    if (data.LAT !== '0.000000') {
        map.setView([parseFloat(data.LAT), parseFloat(data.LON)], 16);
        marker.setLatLng([parseFloat(data.LAT), parseFloat(data.LON)]);
    }
};
```

---

## 5. Hardware/Software Integration Testing Setup

Before field deployment, the following bench tests are performed:

1. **Serial Monitor Verification:** Flash tx-lora.ino and open Serial Monitor at 115200 baud. Verify `SYSTEM READY` message, MPU6050 initialization success, and regular `========== TX DATA ==========` telemetry output every ~1000 ms.

2. **LoRa Link Test:** Flash rx-lora.ino on second ESP32. Verify packet strings appear on base station serial output within 2 seconds of telemetry node startup.

3. **nRF Range Test:** Place TX and RX at progressively greater distances. Verify consistent packet receipt at 10 m, 50 m, and 100 m before field deployment.

4. **Sensor Response Test:** Expose MQ-2 to lighter gas, KY-026 to phone flashlight (IR component), tap SW-420 against palm — verify `ALERT=1` in LoRa packet, Red LED activating, and buzzer activating.

5. **Camera Stream Test:** Connect to ESP32-CAM Wi-Fi network, navigate to `http://<cam_ip>:81/stream` in browser — verify smooth MJPEG feed.

---

*Previous: [08 — Tech Stack ←](./08_Tech_Stack.md) | Next: [10 — Testing & Results →](./10_Testing_and_Results.md)*
