# ⚡ INSTALLATION — RescueBOT Quick Start Guide

> **Time to get running:** ~30 minutes
> **Difficulty:** Intermediate (requires Arduino IDE and Python familiarity)

---

## Prerequisites

### Software
| Tool | Version | Download |
|:---|:---:|:---|
| Arduino IDE | 2.x | [arduino.cc/downloads](https://www.arduino.cc/en/software) |
| Python | 3.9+ | [python.org/downloads](https://www.python.org/downloads/) |
| Node.js | 18 LTS | [nodejs.org](https://nodejs.org/) |
| Git | Latest | [git-scm.com](https://git-scm.com/) |
| Chrome / Firefox | Latest | Any modern browser |

### Arduino Board Cores (install in Arduino IDE → Boards Manager)
| Core | Package URL |
|:---|:---|
| **ESP32** | `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` |
| **Arduino AVR** | Built-in (comes with Arduino IDE) |

### Arduino Libraries (install in Arduino IDE → Library Manager)
| Library | Search Term |
|:---|:---|
| RF24 | `RF24 TMRh20` |
| LoRa | `LoRa sandeepmistry` |
| TinyGPSPlus | `TinyGPSPlus Mikal Hart` |
| Adafruit MPU6050 | `Adafruit MPU6050` |
| Adafruit Unified Sensor | `Adafruit Unified Sensor` |
| ArduinoJson | `ArduinoJson Blanchon` |

> **Alternatively:** All libraries are bundled in `firmware/libraries/` — copy them to your Arduino libraries folder.

---

## Step 1 — Clone the Repository

```bash
git clone https://github.com/Prolayjit-B14/RescueBOT.git
cd RescueBOT
```

---

## Step 2 — Flash the Firmware (5 boards)

Flash each board in order. Open each `.ino` file in Arduino IDE.

### Board 1: ESP32 Telemetry Transmitter
```
File:   firmware/lora_module/tx-lora.ino
Board:  ESP32 Dev Module
Speed:  921600 baud
CPU:    240 MHz (WiFi/BT)
```
> Edit `tx-lora.ino` if you need to change sensor thresholds (lines ~49–51).

### Board 2: ESP32 LoRa Base Station Receiver
```
File:   firmware/lora_module/rx-lora.ino
Board:  ESP32 Dev Module
Speed:  921600 baud
```
> This board plugs into your PC via USB — it bridges LoRa packets to Serial.

### Board 3: Arduino Nano Remote Controller (TX)
```
File:   firmware/nrf_communication/tx.ino
Board:  Arduino Nano
Processor: ATmega328P (Old Bootloader)
Speed:  115200 baud
```

### Board 4: Arduino Nano Chassis Controller (RX)
```
File:   firmware/nrf_communication/rx.ino
Board:  Arduino Nano
Processor: ATmega328P (Old Bootloader)
Speed:  115200 baud
```

### Board 5: ESP32-CAM Video Server
```
File:   firmware/cam_module/production.ino
Board:  AI Thinker ESP32-CAM
Speed:  115200 baud
```
> ⚠️ Edit `production.ino` — set your Wi-Fi SSID and password before flashing:
> ```cpp
> const char* ssid     = "YOUR_WIFI_SSID";
> const char* password = "YOUR_WIFI_PASSWORD";
> ```
> ESP32-CAM requires a USB-TTL adapter for flashing. Connect GPIO0 to GND during upload, release after.

---

## Step 3 — Launch the Web Dashboard

```bash
cd website
npm install
npm run dev
```
Open your browser at `http://localhost:5173` (or whatever port Vite shows).

### Configure the Serial Bridge (for LoRa data)
```bash
pip install pyserial websockets
python website/shared/mqtt-client.py --port COM3   # Windows
python website/shared/mqtt-client.py --port /dev/ttyUSB0  # Linux/Mac
```
> Replace `COM3` with the actual port of your LoRa Base Station ESP32.

---

## Step 4 — Connect the Camera Feed

1. Power on the ESP32-CAM and wait ~5 seconds for Wi-Fi connection.
2. Find the camera's IP address from your router's DHCP table, or check ESP32-CAM serial output.
3. Open `http://<camera_ip>:81/stream` in a browser tab to verify the stream.
4. In the dashboard camera page, enter the camera IP when prompted.

---

## Step 5 — Power On & Test

1. **Remote controller** → Power on (Arduino Nano TX). LED on nRF module should light.
2. **Rover chassis** → Connect battery pack. Arduino Nano RX boots and waits for packets.
3. **Telemetry ESP32** → Powers on with rover. Green LED should be ON (safe state).
4. **Base station** → ESP32 LoRa RX connected to PC shows packets in Serial Monitor.
5. **Dashboard** → Sensor values update every ~1 second. GPS updates when fix acquired.

---

## Verify Everything Works

| Check | How |
|:---|:---|
| Motor control | Move car joystick → wheels turn |
| Fail-safe | Turn off remote → motors stop within 500 ms |
| Gas alert | Hold lighter near MQ-2 → Red LED + buzzer activate |
| Flame alert | Hold lighter near KY-026 → Red LED + buzzer activate |
| LoRa telemetry | Dashboard shows live sensor values |
| Camera | `http://<cam_ip>:81/stream` shows live video |
| GPS | Dashboard map shows pin after outdoor fix (~38 s) |

---

## Troubleshooting Installation

| Problem | Fix |
|:---|:---|
| ESP32 won't upload | Hold BOOT button during upload start |
| ESP32-CAM won't upload | Connect GPIO0 → GND, press RST, release GPIO0 after upload |
| `Library not found` error | Copy `firmware/libraries/*` to Arduino libraries folder |
| Serial port not found | Install CH340G driver (for clone Nanos/ESP32s) |
| Camera shows blank | Supply 5V to ESP32-CAM (not 3.3V) |
| No LoRa packets | Check SX1278 wiring; verify `LoRa.begin(433E6)` returns true |

→ Full troubleshooting: **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)**
→ Hardware wiring: **[HARDWARE_SETUP.md](HARDWARE_SETUP.md)**
