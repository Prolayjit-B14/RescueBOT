# 💻 SOFTWARE SETUP — Firmware Configuration & Dashboard Guide

---

## 1. Arduino IDE Setup

### Install Board Cores

Open Arduino IDE → **File → Preferences → Additional Board Manager URLs**, add:

```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Then go to **Tools → Board → Boards Manager**:
- Search `esp32` → Install **esp32 by Espressif Systems** (v2.x or v3.x)
- Arduino AVR (for Nano) is built-in — no extra install needed

### Install Libraries

**Tools → Manage Libraries**, install:

| Library Name | Search Term | Author |
|:---|:---|:---|
| RF24 | `RF24` | TMRh20 |
| LoRa | `LoRa` | Sandeep Mistry |
| TinyGPSPlus | `TinyGPSPlus` | Mikal Hart |
| Adafruit MPU6050 | `Adafruit MPU6050` | Adafruit |
| Adafruit Unified Sensor | `Adafruit Unified Sensor` | Adafruit |
| ArduinoJson | `ArduinoJson` | Benoit Blanchon |
| Adafruit SSD1306 | `Adafruit SSD1306` | Adafruit |
| Adafruit GFX | `Adafruit GFX` | Adafruit |

> **Alternative:** Copy the contents of `firmware/libraries/` directly into your Arduino `libraries` folder:
> - Windows: `C:\Users\<you>\Documents\Arduino\libraries\`
> - Linux/Mac: `~/Arduino/libraries/`

---

## 2. Firmware Configuration

### 2.1 — Telemetry ESP32 (`firmware/lora_module/tx-lora.ino`)

Configurable thresholds at the top of the file:

```cpp
// =====================================================
// THRESHOLD VALUES — adjust for your environment
// =====================================================
int vibrationThreshold = 1500;   // SW-420 raw ADC alert level
int gasThreshold       = 1800;   // MQ-2 raw ADC alert level
int distanceThreshold  = 20;     // HC-SR04 obstacle distance (cm)
// IMU threshold: |ax| or |ay| > 15 m/s² (coded in loop)
```

**Board settings in Arduino IDE:**
```
Board:         ESP32 Dev Module
CPU Frequency: 240 MHz (WiFi/BT)
Flash Mode:    QIO
Flash Size:    4MB (32Mb)
Upload Speed:  921600
Port:          COMx (your port)
```

### 2.2 — LoRa Base Station (`firmware/lora_module/rx-lora.ino`)

No configuration needed. Just flash and leave connected to PC via USB — it acts as a serial bridge, forwarding LoRa packets to the dashboard.

**Board settings:** Same as tx-lora (ESP32 Dev Module).

### 2.3 — Remote Controller (`firmware/nrf_communication/tx.ino`)

Deadzone and smoothing can be tuned:

```cpp
#define DEADZONE_CAR  90    // ±90 around center (512) for car joystick
#define DEADZONE_ARM  70    // ±70 around center (512) for arm joystick
// Smoothing coefficient: 0.35 = 35% new reading weight
float smooth = (raw - smooth) * 0.35 + smooth;
```

**Board settings:**
```
Board:     Arduino Nano
Processor: ATmega328P (Old Bootloader)   ← use this for clone Nanos
Port:      COMx
```

### 2.4 — Chassis Controller (`firmware/nrf_communication/rx.ino`)

Motor speeds and fail-safe timeout:

```cpp
#define MOTOR_SPEED_DRIVE  120   // PWM (0-255) for forward/backward
#define MOTOR_SPEED_TURN   100   // PWM for turning (lower = better traction)
#define TIMEOUT_MS         500   // ms before fail-safe motor halt
```

**Board settings:** Same as tx.ino (Arduino Nano).

### 2.5 — ESP32-CAM (`firmware/cam_module/production.ino`)

**⚠️ Edit Wi-Fi credentials before flashing:**

```cpp
const char* ssid     = "YOUR_WIFI_SSID";      // ← change this
const char* password = "YOUR_WIFI_PASSWORD";   // ← change this
```

**Flashing ESP32-CAM** (requires USB-TTL adapter like CH340G/CP2102):
```
TX (USB-TTL) → U0RXD (ESP32-CAM)
RX (USB-TTL) → U0TXD (ESP32-CAM)
GND          → GND
5V           → 5V
GPIO0        → GND   ← connect ONLY during upload, disconnect after
```

1. Connect GPIO0 → GND
2. Press RST button on ESP32-CAM
3. Click **Upload** in Arduino IDE
4. When upload reaches ~5%, release RST
5. After upload completes, disconnect GPIO0 from GND
6. Press RST again → camera boots normally

**Board settings:**
```
Board:      AI Thinker ESP32-CAM
Port:       COMx (USB-TTL adapter port)
```

---

## 3. Dashboard Setup

### 3.1 — Install Dependencies

```bash
cd website
npm install
```

### 3.2 — Start Development Server

```bash
npm run dev
```

Open: `http://localhost:5173`

### 3.3 — Configure Serial Bridge

The LoRa Base Station ESP32 connects to your PC via USB. A Python script reads the serial port and pushes data to the dashboard:

```bash
pip install pyserial websockets
```

Find your serial port:
- **Windows:** Device Manager → Ports → `COMx`
- **Linux:** `ls /dev/ttyUSB*` or `ls /dev/ttyACM*`
- **Mac:** `ls /dev/cu.*`

Run the bridge:
```bash
python website/shared/mqtt-client.py --port COM3      # Windows
python website/shared/mqtt-client.py --port /dev/ttyUSB0  # Linux
```

### 3.4 — Embed Camera Feed

In the dashboard camera page, enter the ESP32-CAM's IP address when prompted. The stream endpoint is:
```
http://<ESP32-CAM-IP>:81/stream
```

To find the camera's IP: check your router's DHCP client list, or read the Serial Monitor output when the CAM boots (it prints its IP address).

---

## 4. Testing Firmware (Serial Monitor)

### Telemetry ESP32 Output (expected)
Open Serial Monitor at **115200 baud** after flashing `tx-lora.ino`:

```
==========================
SYSTEM STARTING...
==========================
Checking MPU6050...
MPU6050 OK
GPS Started
Starting LoRa...
LoRa Started
SYSTEM READY

========== TX DATA ==========
VIB=12,GAS=412,FL1=1,FL2=1,DIST=85.00,AX=0.12,AY=-0.08,AZ=9.79,LAT=0.000000,LON=0.000000,ALERT=0
=============================
```

> `LAT=0.000000` is normal until the GPS gets an outdoor satellite fix (~38 seconds).

### LoRa Receiver Output (expected)
On the base station ESP32 Serial Monitor at **115200 baud**:

```
LoRa Receiver Ready
Received packet:
VIB=12,GAS=412,FL1=1,FL2=1,DIST=85.00,...,ALERT=0
RSSI: -72
```

---

## 5. Common Configuration Mistakes

| Mistake | Symptom | Fix |
|:---|:---|:---|
| Wrong board selected | Upload fails / wrong pins | Match board name exactly to hardware |
| Old Bootloader not selected for Nano clone | `avrdude: stk500_recv()` error | Select `ATmega328P (Old Bootloader)` |
| GPIO0 not connected to GND on CAM | Upload fails immediately | Connect GPIO0 → GND before upload |
| Wi-Fi credentials not updated in CAM firmware | Camera never connects | Edit ssid/password in `production.ino` |
| Wrong serial port | Port not found error | Check Device Manager / `dmesg` |
| Library version mismatch | Compile errors | Use library versions from `firmware/libraries/` |

---

*See also: [INSTALLATION.md](INSTALLATION.md) for the full quick-start flow.*
*See also: [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for runtime issue fixes.*
