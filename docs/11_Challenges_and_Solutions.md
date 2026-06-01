# 🛠️ 11 — Challenges & Solutions: Engineering Hurdles, Root Causes & Implemented Fixes

---

## Overview

The development of RescueBOT encountered a series of non-trivial engineering challenges spanning hardware conflicts, signal interference, power management, sensor calibration, and firmware timing. Each challenge required systematic diagnosis, root-cause analysis, and an engineered solution. This document provides a detailed technical record of each problem and its resolution — serving both as a lessons-learned resource and as a demonstration of the team's embedded systems engineering capability.

---

## 1. Challenge-Solution Matrix

| # | Category | Challenge | Root Cause | Implemented Solution | Impact |
|:---:|:---|:---|:---|:---|:---|
| 1 | **RF Control** | Motor control jitter and erratic UGV movement at joystick neutral position | Raw ADC noise (±15–20 LSB) from joystick potentiometers at center position causing false non-zero values | Implemented exponential moving average filter (α=0.35) + deadzone (±90 units around 512) in tx.ino | Eliminated motor creep at neutral; smooth control response |
| 2 | **RF Interference** | nRF24L01+ packet loss when operating near venue Wi-Fi access points | nRF24L01+ default channels (1–76) overlap with 2.4 GHz Wi-Fi channels 1, 6, 11 at hackathon venue | Configured nRF24L01+ to Channel 108 (2508 MHz) — above all standard 2.4 GHz Wi-Fi channels (1–13 = 2412–2472 MHz) | Packet loss dropped from ~30% to <1% |
| 3 | **RF Architecture** | Control packets and sensor telemetry interfering when both used 2.4 GHz nRF | Both control and data on same frequency band caused mutual interference in close proximity | Implemented dual-band architecture: control on 2.4 GHz (nRF24L01+), sensor telemetry on 433 MHz (SX1278 LoRa) | Zero cross-interference; both links now operate independently |
| 4 | **ESP32-CAM** | Random camera freezes, blank frames, and full board reboots during telemetry operation | GPIO 16 on ESP32-CAM is hardwired to PSRAM clock. HC-SR04 Echo pin was connected to GPIO 16, causing PSRAM clock corruption on every echo pulse | Isolated ESP32-CAM as a completely standalone node. All sensors moved exclusively to Telemetry ESP32. ESP32-CAM has zero sensor connections. | Camera system runs stable with no freezes or reboots |
| 5 | **Sensor Conflict** | Flame Sensor 2 triggering random false alerts | GPIO 26 was shared between Flame Sensor 2 digital input and the SX1278 LoRa receiver's DIO0 interrupt line on the base station ESP32. Every LoRa packet reception caused a GPIO 26 pulse, interpreted as a flame event | Reassigned Flame Sensor 2 from GPIO 26 to GPIO 33 in both firmware (tx-lora.ino) and physical circuit connections | No false flame alerts; LoRa DIO0 interrupt functions correctly on GPIO 26 |
| 6 | **Power Management** | Microcontroller resets during aggressive UGV acceleration maneuvers | Motor startup inrush current (estimated 500–800 mA peak per motor × 4 = up to 3.2 A peak) caused a voltage dip on the shared power rail, triggering ATmega328P and ESP32 brownout resets (threshold: ~2.8–3.0V) | Separated power architecture: DC motors connected directly to raw 7.4V–8.4V battery rail; all MCUs and sensors powered from a dedicated 5V buck converter output. Added 10 µF electrolytic bulk capacitor at 5V rail. | Zero brownout resets; stable MCU operation during full-throttle acceleration |
| 7 | **Gas Sensor** | MQ-2 returning inverted concentration readings (very high ADC in clean air, low ADC in gas presence) | MQ-2 comparator boards from some manufacturers use active-HIGH output (pull-up configuration), causing the voltage to be high in clean air and drop in gas presence — opposite of standard MQ-2 behavior | Implemented automatic board variant detection in firmware: if analogRead(MQ2_PIN) > 3000 in initial clean-air readings, apply: `gasValue = 4095 - rawValue` | Firmware now compatible with both active-high and active-low MQ-2 board variants without hardware changes |
| 8 | **Analog Noise** | High-frequency noise in MQ-2 and SW-420 analog readings during motor operation | Motor and servo PWM switching (490 Hz) induces electromagnetic interference onto shared ground rail and adjacent analog signal wires. Motor brushes generate RF noise across frequency spectrum. | Added 100 nF ceramic decoupling capacitors at MQ-2 and SW-420 VCC pins. Routed analog sensor wires away from motor power cables. Applied software exponential filter to sensor readings. | Reduced noise floor by ~60%; stable, consistent sensor readings |
| 9 | **Servo Jitter** | Robotic arm servos twitching slightly at rest, even with no joystick movement | Residual ADC noise (~5–10 LSB) on arm joystick channels producing small angle increment commands even in neutral | Added ±70 unit deadzone (around center 512) for arm joystick channels in tx.ino. Angle increment only applied if joystick offset exceeds deadzone threshold. | Arm servos hold position cleanly at rest |
| 10 | **Loop Blocking** | ESP32 firmware hanging for 20–30 seconds when no ultrasonic echo was received (e.g., UGV pointing at open ceiling) | `pulseIn(ECHO_PIN, HIGH)` without timeout argument blocks indefinitely until echo pulse arrives. With no obstacle in range (>400 cm), the HC-SR04 never produces an echo, causing a permanent loop freeze. | Added 30,000 µs (30 ms) timeout to all `pulseIn()` calls: `pulseIn(ECHO_PIN, HIGH, 30000)`. Returns 0 on timeout; firmware assigns `distance = 999.0 cm` on timeout. | Zero loop hangs; telemetry continues normally with 999 cm as "no obstacle" value |
| 11 | **GPS Indoor** | GPS module not acquiring a satellite fix when tested indoors | NEO-6M requires line-of-sight to at least 4 GPS satellites. Building ceilings and walls attenuate GPS L1 (1575.42 MHz) signals below acquisition threshold. | GPS testing and pre-deployment satellite acquisition must be done outdoors or near large windows. Documentation updated to specify outdoor GPS lock procedure before field deployment. | GPS reliable outdoors; expected limitation for indoor operation |
| 12 | **ESP32-CAM Boot** | ESP32-CAM failing to start when powered from the UGV's 3.3V regulator rail | The AI-Thinker ESP32-CAM requires a 5V VCC input to its onboard AMS1117-3.3 regulator, which then provides 3.3V to the ESP32 module. Directly connecting 3.3V to the VCC pin bypasses the LDO and under-powers the module during radio TX events. | Connected ESP32-CAM to the 5V regulated rail (or separate USB power bank for field tests). Verified 5V is within the board's 4.75V–5.5V acceptable range. | Stable ESP32-CAM boot and operation |

---

## 2. Detailed Technical Analysis: Top 3 Critical Challenges

### 2.1 The ESP32-CAM GPIO 16 Conflict — Architecture-Level Resolution

This was the most disruptive hardware conflict encountered during development. Initial prototype design placed all sensors on a single ESP32-CAM board to minimize component count. The HC-SR04 Echo pin was wired to GPIO 16 (a seemingly available GPIO).

**The underlying cause:** The AI-Thinker ESP32-CAM module connects the OV2640 camera sensor's pixel data interface to 8 ESP32 GPIOs. More critically, the board uses **GPIO 16 as the PSRAM (Pseudo-SRAM) chip select / clock signal** for the board's external 8 MB PSRAM. This connection is hardwired on the PCB and cannot be changed in software.

Every time the HC-SR04 echo pulse occurred, it drove GPIO 16 HIGH momentarily, which the PSRAM controller interpreted as a spurious clock edge, corrupting the PSRAM buffer containing the camera framebuffer. The result: camera frame corruption, DMA errors, and watchdog timer resets.

**The systemic fix:** Redesigning the entire system around the principle of **subsystem isolation** — dedicating a separate ESP32 DevKit exclusively to sensors (no camera) and reserving the ESP32-CAM exclusively for video (no sensors). This architectural change also improved system maintainability, as each subsystem can now be independently rebooted, debugged, or replaced.

**Lesson:** When using any ESP32-CAM board variant, **never use GPIO 16 as a user GPIO**. Extend this caution to GPIO 4 (already occupied by flash LED), GPIO 0 (boot mode select), and the 8-pin OV2640 camera data bus.

### 2.2 The Dual-Band RF Architecture — Signal Integrity Through Frequency Separation

Initial prototypes used a single nRF24L01+ for both control commands and sensor data in alternating packets. Under clean lab conditions this worked. At the hackathon venue — surrounded by multiple active 2.4 GHz Wi-Fi access points — packet collision rates exceeded 30%, causing erratic UGV motion and sensor data gaps.

**The analysis:** The 2.4 GHz ISM band (2400–2483.5 MHz) is heavily congested. Wi-Fi channels 1 (2412 MHz), 6 (2437 MHz), and 11 (2462 MHz) each have 22 MHz bandwidth, occupying channels 1–76 of the nRF24L01+'s addressable range in Wi-Fi-heavy environments. Additionally, sending both control and data on the same nRF link introduced a latency tradeoff: prioritizing control packets delayed sensor data, while prioritizing sensor data caused control jitter.

**The solution's engineering rationale:** 433 MHz sub-GHz signals experience approximately 20 dB lower free-space path loss than 2.4 GHz at equivalent distances (per Friis transmission equation: path loss ∝ frequency²). Additionally, the 433 MHz band has far fewer competing devices in typical disaster environments, and concrete/masonry attenuate it approximately 3× less than 2.4 GHz. This makes LoRa at 433 MHz ideal for long-range, obstacle-penetrating telemetry, while the faster nRF24L01+ (250 kbps vs. LoRa's ~1 kbps effective rate) is kept exclusively for time-critical control.

### 2.3 Motor Startup Brownout — Power Architecture Validation

Voltage measurements during the brownout investigation revealed the shared power rail dropping from 5.0V to 3.1V in approximately 8 ms during simultaneous full-throttle activation of all four motors. The ATmega328P's minimum operating voltage is 2.7V at 8 MHz (4.5V at 16 MHz per datasheet), meaning this voltage dip caused immediate processor reset at the 16 MHz operating frequency.

**The quantified solution:** The raw battery rail (7.4V+ source) has sufficiently low source impedance for the motor current spikes. The regulated 5V rail's buck converter maintains stable output regardless of load transients on the motor rail, as the two rails share only the negative (GND) bus — which is low impedance by design. Post-fix measurements confirmed the 5V rail remained at 4.95V–5.05V during all motor acceleration events.

---

## 3. Technical Takeaways for Future Versions

1. **Always isolate high-power loads (motors, servos) onto dedicated power rails** separated from sensitive logic electronics.
2. **Study GPIO functional multiplexing before wire assignment** — SoC peripheral documentation (especially for ESP32) lists pins with restricted functions that cannot be overridden.
3. **Use separate frequency bands for control and data** in multi-channel wireless systems operating in congested RF environments.
4. **Always add timeouts to all blocking I/O calls** (`pulseIn`, `Serial.read`, `Wire.requestFrom`). A single missing timeout can freeze the entire firmware loop.
5. **Software filters reduce hardware requirements** — a first-order IIR filter eliminates RC filter components for joystick noise, reducing BOM cost and board complexity.

---

*Previous: [10 — Testing & Results ←](./10_Testing_and_Results.md) | Next: [12 — Future Scope →](./12_Future_Scope.md)*
