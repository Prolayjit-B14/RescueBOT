# ❓ FAQ — Frequently Asked Questions

---

## General

### Q: What is RescueBOT?
**A:** RescueBOT is an open-source autonomous disaster response UGV (Unmanned Ground Vehicle) built with ESP32, Arduino Nano, LoRa 433 MHz, and nRF24L01+ RF modules. It detects gas, flame, vibration, and obstacles in real time, streams live video, and tracks GPS coordinates — all for under ₹10,000 (~USD 120).

### Q: What disasters is RescueBOT designed for?
**A:** Earthquakes, structural fires, industrial gas leaks, building collapses, floods, and mine disasters. Any environment where hazard exposure makes it unsafe for human rescuers to enter first.

### Q: What is the total cost to build RescueBOT?
**A:** The complete bill of materials costs approximately **₹5,590–₹9,970** depending on your component suppliers. The detailed BOM is in [`HARDWARE_SETUP.md`](HARDWARE_SETUP.md) and [`hardware/Cost_Analysis.md`](hardware/Cost_Analysis.md).

### Q: Is this project open source?
**A:** Yes — fully open source under the **MIT License**. You can use, modify, and build on it for personal, educational, or commercial purposes with attribution.

### Q: Where was RescueBOT built and demonstrated?
**A:** By **Team BOT THINGS** at the **ZYRO 2026 Hackathon**, Kalyani Government Engineering College (KGEC), West Bengal, India.

---

## Hardware

### Q: Can I use a different ESP32 board instead of the WROOM-32?
**A:** Yes. Any ESP32 board with the ESP32-WROOM-32 module (ESP32-WROOM-32D, -32E, etc.) will work. Avoid ESP32-S2 or ESP32-S3 without modifying the firmware — they have different peripheral mappings.

### Q: Why does the ESP32-CAM need a separate 5V supply?
**A:** The AI-Thinker ESP32-CAM board's onboard AMS1117-3.3 LDO regulator requires 5V input to properly power the ESP32 module. If you supply 3.3V directly, the module will be under-powered, causing random reboots especially during Wi-Fi radio bursts. Always supply **5V to the 5V pin** (not the 3V3 pin) of the ESP32-CAM.

### Q: Why can't I use GPIO 16 on the ESP32-CAM?
**A:** GPIO 16 is wired to the PSRAM (external pseudo-SRAM) clock signal on the AI-Thinker ESP32-CAM PCB. Using it for any other peripheral corrupts the PSRAM buffer, which stores the camera framebuffer — causing blank frames, DMA errors, and watchdog resets. **Never connect anything to GPIO 16 on the ESP32-CAM.**

### Q: Can I use a single ESP32 for both sensors and the camera?
**A:** No — not on the AI-Thinker ESP32-CAM. The GPIO 16 PSRAM conflict makes it impossible to safely attach HC-SR04 (or most other sensors) to the same board as the camera. This is why RescueBOT uses a separate ESP32 DevKit exclusively for sensors and the ESP32-CAM exclusively for video.

### Q: Can I replace nRF24L01+ with something else for control?
**A:** Yes. Possible alternatives: **433 MHz HC-12 module** (simpler UART interface, longer range, but higher latency), **ESP-NOW** (ESP32-to-ESP32 Wi-Fi peer protocol, very low latency), or **2.4 GHz NRF52** modules. Each requires firmware changes. The nRF24L01+ PA+LNA is recommended for its proven 200 m+ range and 35 ms control latency.

### Q: What is the maximum control range?
**A:** In open-air LOS: approximately **180–200 m** with the nRF24L01+ PA+LNA at 250 kbps on Channel 108. In urban/indoor environments, expect 50–100 m depending on wall materials.

### Q: What is the maximum telemetry range?
**A:** The SX1278 LoRa at 433 MHz achieves approximately **350 m in urban environments** (multiple walls) and **1.2 km line-of-sight**. In the KGEC testing, reliable packets were received through 3 stacked concrete walls (RSSI −98 dBm).

### Q: Can the robotic arm lift heavy objects?
**A:** The MG90S servos provide ~1.8 kg·cm torque. The arm was validated to lift **100 g test weights**. Heavier payloads require upgraded servos (MG996R provides ~10 kg·cm). See [`docs/12_Future_Scope.md`](docs/12_Future_Scope.md) for the relay-based payload drop upgrade.

---

## Firmware

### Q: Why does the gas sensor show very high readings in clean air?
**A:** Some MQ-2 comparator boards use an **active-high configuration** — the output voltage is high in clean air and drops with gas exposure (inverted behavior). The firmware automatically detects this: if the baseline reading in clean air exceeds 3000 ADC, it applies `gasValue = 4095 - rawValue` to invert the reading. No hardware changes are needed.

### Q: Why does my ultrasonic sensor cause the firmware to freeze?
**A:** If `pulseIn()` is called without a timeout, it blocks indefinitely when no echo is received (e.g., when the UGV faces an open space far beyond 400 cm). Ensure your `pulseIn()` call includes the 30 ms timeout:
```cpp
long duration = pulseIn(ECHO_PIN, HIGH, 30000);
```
This is already implemented in `tx-lora.ino`.

### Q: The joystick causes motor jitter at the neutral position. How do I fix it?
**A:** Ensure the **deadzone** values in `tx.ino` are large enough to absorb joystick ADC noise. The default is ±90 around the centre (512):
```cpp
if (abs(smooth - 512) < DEADZONE_CAR) smooth = 512;
```
Increase `DEADZONE_CAR` to `120` if jitter persists.

### Q: The UGV doesn't stop when I turn off the remote. Is the fail-safe working?
**A:** Confirm `TIMEOUT_MS = 500` is defined in `rx.ino` and the timeout check runs every loop iteration. If the motors don't stop, check that the nRF24L01+ isn't still receiving packets from another device (wrong address pipe). Verify the address bytes match between `tx.ino` and `rx.ino`.

### Q: My Arduino Nano clone gives `avrdude: stk500_recv()` errors during upload.
**A:** Select **ATmega328P (Old Bootloader)** in Tools → Processor. Most Chinese clone Nanos with CH340G USB chips use the older bootloader.

### Q: The ESP32-CAM won't enter upload mode.
**A:** Ensure:
1. GPIO0 is connected to GND **before** pressing RST.
2. You're using a USB-TTL adapter that provides **5V** (not 3.3V) to the CAM's 5V pin.
3. The baud rate in Arduino IDE is set to **115200**.
4. No other program is occupying the serial port.

---

## Dashboard

### Q: The dashboard shows no data / all zeros.
**A:** Check that:
1. The LoRa base station ESP32 (rx-lora) is connected to your PC via USB.
2. The Python serial bridge script is running and pointing to the correct COM port.
3. The telemetry ESP32 (tx-lora) is powered on and the Serial Monitor shows `TX DATA` packets.

### Q: The GPS coordinates on the map don't update.
**A:** The NEO-6M GPS requires an **outdoor satellite fix** (cold start ~38 seconds). Until a fix is acquired, `LAT=0.000000, LON=0.000000` is sent in every packet. Take the UGV outdoors and wait for the green LED on the GPS module to flash at 1 Hz (fix acquired).

### Q: The camera feed shows "Not Connected" or a blank image.
**A:** Check:
1. ESP32-CAM is powered with 5V and connected to the same Wi-Fi network as your PC.
2. The IP address entered in the dashboard matches the camera's actual IP.
3. The stream URL is `http://<IP>:81/stream` (not HTTPS).
4. No browser extension is blocking the HTTP stream.

---

## Contributing & Community

### Q: Can I contribute hardware improvements or new sensors?
**A:** Absolutely. Read [`CONTRIBUTING.md`](CONTRIBUTING.md) for the full contribution workflow. For hardware changes, update [`circuit_diagram/pin_connections.md`](circuit_diagram/pin_connections.md) and the affected firmware file.

### Q: I found a security vulnerability. How do I report it?
**A:** Do **not** open a public Issue. Follow the instructions in [`SECURITY.md`](SECURITY.md) to report privately to the maintainer.

### Q: I have a question not answered here.
**A:** Open a [Discussion](https://github.com/Prolayjit-B14/RescueBOT/discussions) on GitHub — the team monitors it actively.
