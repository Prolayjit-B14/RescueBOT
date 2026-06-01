# 🛠️ TROUBLESHOOTING — Known Issues, Diagnoses & Fixes

---

## Quick Diagnostic Checklist

Before diving into specific issues, run through this checklist:

- [ ] All boards are powered (check LEDs and serial output)
- [ ] Common ground bus connects all modules to the same GND reference
- [ ] Correct firmware flashed to correct board
- [ ] Serial Monitor baud rate set to **115200**
- [ ] nRF24L01+ powered from **3.3V** (not 5V)
- [ ] ESP32-CAM powered from **5V** (not 3.3V)
- [ ] GPIO 16 is NOT used for any peripheral on ESP32-CAM

---

## 1. Firmware Upload Issues

### ❌ `avrdude: stk500_recv(): programmer is not responding`

| Cause | Fix |
|:---|:---|
| Clone Arduino Nano with old bootloader | Tools → Processor → **ATmega328P (Old Bootloader)** |
| Wrong COM port selected | Device Manager → check port; select correct COMx |
| USB cable is charge-only | Use a data-capable USB cable |
| Another program holds the port | Close Serial Monitor; close other IDEs |

---

### ❌ ESP32 Upload Fails / `Connecting...` Hangs

| Cause | Fix |
|:---|:---|
| ESP32 not entering bootloader | Hold **BOOT** button while clicking Upload; release when upload progress shows |
| Wrong board selected | Tools → Board → `ESP32 Dev Module` (not ESP32-S2/S3) |
| CH340G driver not installed | Install CH340G driver; restart PC |
| Upload speed too high | Reduce to 460800 baud if 921600 fails |

---

### ❌ ESP32-CAM Won't Upload

| Cause | Fix |
|:---|:---|
| GPIO0 not connected to GND | Connect GPIO0 → GND **before** power/RST |
| 3.3V from USB-TTL (not enough) | Use USB-TTL that supplies **5V** on VCC pin |
| USB-TTL TX/RX swapped | TX (adapter) → U0RXD (CAM); RX (adapter) → U0TXD (CAM) |
| Port not recognized | Install CP2102 or CH340G driver matching your adapter chip |

---

### ❌ `Library not found: RF24.h` / `LoRa.h`

| Cause | Fix |
|:---|:---|
| Library not installed | Library Manager → install RF24, LoRa, TinyGPSPlus, Adafruit MPU6050 |
| Manual install location wrong | Copy `firmware/libraries/*` → `Documents/Arduino/libraries/` |
| Board mismatch | RF24 must be installed for the ESP32/AVR core you're using |

---

## 2. Sensor Problems

### ❌ MQ-2 Gas Always Shows Alert (High ADC in Clean Air)

**Diagnosis:** `GAS=3800` or similar in serial output with no gas present.

**Cause:** Active-high MQ-2 board variant — output voltage is HIGH in clean air, LOW with gas (inverted from standard behaviour).

**Fix:** The firmware auto-detects this. Ensure you're running the latest `tx-lora.ino`:
```cpp
// Auto-inversion is applied when baseline > 3000 in clean air
if (gasValue > 3000) gasValue = 4095 - gasValue;
```
If this isn't in your firmware, add it. Also allow **20–30 minutes preheat** before calibrating baseline.

---

### ❌ Ultrasonic Distance Always Reads 0 or 999

| Symptom | Cause | Fix |
|:---|:---|:---|
| Always `0` | Echo pin shorted to GND, or `pulseIn` returning 0 (no echo) | Check ECHO wiring; increase `pulseIn` timeout to 35000 µs |
| Always `999` | No echo received (object out of range or sensor facing open space) | This is correct behaviour — `999` = no obstacle detected |
| Erratic readings | Motor EMI on signal wire | Route ECHO/TRIG wires away from motor cables; add 100nF decoupling at HC-SR04 VCC |

---

### ❌ MPU6050 Not Found — `Failed to find MPU6050`

| Cause | Fix |
|:---|:---|
| I2C pull-up resistors missing | Add 4.7 kΩ from SDA and SCL to 3.3V |
| Wrong I2C address | Default is 0x68; if AD0 pin is HIGH it becomes 0x69 — match in code |
| SDA/SCL swapped | GPIO 21 = SDA, GPIO 22 = SCL on ESP32 |
| Insufficient power | Use ESP32's 3.3V pin — do NOT use 5V for MPU6050 |

---

### ❌ GPS Never Gets a Fix

| Cause | Fix |
|:---|:---|
| Tested indoors | GPS requires outdoor sky view — test outdoors or near large windows |
| Antenna not facing skyward | Rotate GPS module so ceramic antenna faces up |
| Cold start time | Allow 45–60 seconds for first fix after power-on |
| Baud rate mismatch | Firmware uses 9600 baud; some modules ship at 38400 — check with `u-center` software |

---

### ❌ Flame Sensor False Triggers (No Fire Present)

| Cause | Fix |
|:---|:---|
| GPIO 26 conflict with LoRa DIO0 | Ensure Flame Sensor 2 is on GPIO **33** (not 26) — see `tx-lora.ino` |
| Sunlight interference | KY-026 responds to 760–1100 nm IR; direct sunlight triggers it |
| Sensitivity pot set too high | Adjust the blue trimmer potentiometer on KY-026 board clockwise to decrease sensitivity |

---

## 3. RF Communication Problems

### ❌ No Motor Response from Remote Controller

| Cause | Fix |
|:---|:---|
| nRF24L01+ powered from 5V | Must use **3.3V** — 5V will damage/destroy the module |
| Channel mismatch | Both TX and RX must use channel 108 — check `radio.setChannel(108)` in both `tx.ino` and `rx.ino` |
| Address mismatch | TX and RX must use identical pipe addresses |
| SPI pins wrong on Nano | CE=7, CSN=8 on RX; CE=9, CSN=10 on TX — verify wiring |
| No 10µF cap on nRF VCC | nRF24L01+ draws 115 mA burst on TX — add 10µF electrolytic at 3.3V pin |

---

### ❌ Fail-Safe Not Triggering (Motors Keep Running After Remote Off)

| Cause | Fix |
|:---|:---|
| Timeout not checked in loop | Ensure `if (millis() - lastReceiveTime > TIMEOUT_MS)` is inside `loop()` |
| `lastReceiveTime` never updated | Confirm `radio.available()` block updates `lastReceiveTime = millis()` |
| Another nRF device on same address | Change the 5-byte pipe address to something unique in both tx/rx firmware |

---

### ❌ No LoRa Packets Received at Base Station

| Cause | Fix |
|:---|:---|
| Frequency mismatch | Both tx-lora and rx-lora must use `LoRa.begin(433E6)` |
| SPI wiring error | Verify SCK/MISO/MOSI/NSS/RST/DIO0 against `pin_connections.md` |
| LoRa.begin() returns false | Add `if (!LoRa.begin(433E6)) { Serial.println("LoRa FAIL"); while(1); }` to diagnose |
| Modules too close (signal saturation) | Move TX and RX at least 1 metre apart during bench testing |

---

## 4. Camera Problems

### ❌ ESP32-CAM Keeps Rebooting / Watchdog Reset

| Cause | Fix |
|:---|:---|
| GPIO 16 used for sensor | Remove all sensor connections from GPIO 16 — it's the PSRAM clock |
| 3.3V supply (insufficient) | Power ESP32-CAM from **5V** |
| PSRAM not enabled in firmware | Config uses `PSRAM_MODE_OCT` or `PSRAM_MODE_SPI` — check `esp_camera_init()` config |

---

### ❌ Camera Shows Blank/Frozen Frames

| Cause | Fix |
|:---|:---|
| GPIO 16 PSRAM corruption | Disconnect any peripheral from GPIO 16 |
| Insufficient current from USB-TTL | Power ESP32-CAM from a proper 5V 1A+ supply during operation |
| Wi-Fi signal too weak | Move CAM closer to router during initial testing |

---

### ❌ Can't Connect to Camera Stream

| Cause | Fix |
|:---|:---|
| Wrong IP address | Check serial output when CAM boots — it prints the IP |
| PC and CAM on different networks | Both must be on the same Wi-Fi SSID |
| Port 81 blocked by firewall | Temporarily disable Windows Firewall for testing |
| HTTPS in browser URL | Use `http://` (not `https://`) — the stream is plain HTTP |

---

## 5. Power Problems

### ❌ MCU Resets When Motors Start

| Cause | Fix |
|:---|:---|
| Shared power rail with motors | Separate rails: motors on raw battery, MCUs on 5V buck converter output |
| Buck converter undersized | Use LM2596 or XL4016 rated ≥ 3A to handle servo + Arduino + sensor loads |
| Missing bulk capacitor | Add 10µF electrolytic at buck converter output |

---

### ❌ Servos Twitching/Jittering at Rest

| Cause | Fix |
|:---|:---|
| Arm joystick ADC noise | Increase `DEADZONE_ARM` to 100 in `tx.ino` |
| Motor PWM switching noise on servo VCC | Separate servo power from motor driver power |
| Signal wire running parallel to motor cable | Re-route servo signal wires away from L298N/motor wiring |

---

## 6. Dashboard Problems

### ❌ Dashboard Shows No Data

| Checklist | |
|:---|:---|
| ☐ | `rx-lora.ino` flashed and ESP32 connected via USB |
| ☐ | Python serial bridge script running with correct COM port |
| ☐ | `tx-lora.ino` powered and showing `TX DATA` in Serial Monitor |
| ☐ | WebSocket URL in dashboard config matches bridge port |

---

### ❌ GPS Map Pin Not Moving

GPS sends `LAT=0.000000` until a satellite fix is acquired outdoors. This is expected. Take the UGV outside and allow up to 60 seconds for first fix.

---

## Still Stuck?

- 📖 **Docs:** [`docs/11_Challenges_and_Solutions.md`](docs/11_Challenges_and_Solutions.md) — detailed root-cause analysis of all known hardware/firmware issues
- 💬 **Ask:** [GitHub Discussions](https://github.com/Prolayjit-B14/RescueBOT/discussions) — post your issue with Serial Monitor output, board type, and wiring photos
- 🐛 **Report a bug:** [GitHub Issues](https://github.com/Prolayjit-B14/RescueBOT/issues)
