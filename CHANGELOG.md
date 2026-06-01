# 📋 CHANGELOG — RescueBOT

All notable changes to RescueBOT are documented in this file.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) — versions use **[vMAJOR.MINOR.PATCH]** semantic versioning.

---

## [Unreleased]

### Planned
- PIR passive infrared human presence detection integration into telemetry firmware
- OLED SSD1306 local display activation in `tx-lora.ino`
- AES-128 encryption layer for LoRa telemetry packets
- Relay module integration for one-shot payload drop mechanism

---

## [v1.2.0] — 2026-06-01 · ZYRO 2026 Final Submission

### Added
- ✅ Complete 14-file professional documentation suite (`docs/01` through `docs/14`)
- ✅ `circuit_diagram/circuit_explanation.md` — full system wiring and functional block documentation
- ✅ `circuit_diagram/pin_connections.md` — complete pin mapping tables for all 5 boards
- ✅ `presentations/` folder with PDF and PPTX presentation files
- ✅ `media/` folder with logo, component list, cost analysis, and pin connection images
- ✅ `demo/` folder with prototype photos and demo videos (stored via Git LFS)
- ✅ `hackathon_gallery/` — ZYRO 2026 event photos and night mode demo video
- ✅ `firmware/libraries/` — local copies of RF24, LoRa, TinyGPS++, Adafruit MPU6050 libraries
- ✅ `firmware/sensor_module/sensors.ino` — Wi-Fi/MQTT alternative sensor test sketch
- ✅ `website/` — full multi-page web dashboard (dashboard, sensors, camera, alerts, map pages)
- ✅ `README.md`, `CHANGELOG.md`, `ROADMAP.md`, `INSTALLATION.md`, `HARDWARE_SETUP.md`, `SOFTWARE_SETUP.md`, `FAQ.md`, `TROUBLESHOOTING.md`, `CODE_OF_CONDUCT.md`, `SECURITY.md`

### Changed
- Reorganized repository structure — removed legacy `images/`, `future_updates/`, `reports/`, `team/`, `datasets/` folders
- Moved presentation files from `presentation/` to `presentations/`
- Renamed `firmware/lora_module/lora_module.ino` → `tx-lora.ino` + `rx-lora.ino` (split TX/RX)
- Renamed `firmware/nrf_communication/nrf_communication.ino` → `tx.ino` + `rx.ino` (split TX/RX)
- Updated `CONTRIBUTING.md` with ESP32-CAM GPIO 16 constraint and non-blocking execution standards

### Fixed
- Flame Sensor 2 GPIO conflict: moved from GPIO 26 to GPIO 33 (was sharing with LoRa DIO0)

---

## [v1.1.0] — 2026-05-28 · Hardware Integration Milestone

### Added
- `firmware/lora_module/tx-lora.ino` — Telemetry ESP32 firmware (sensor fusion + LoRa TX)
- `firmware/lora_module/rx-lora.ino` — Base station LoRa receiver firmware
- `firmware/nrf_communication/tx.ino` — Remote controller Arduino Nano firmware
- `firmware/nrf_communication/rx.ino` — Chassis motor + servo controller firmware
- `firmware/cam_module/production.ino` — Standalone ESP32-CAM MJPEG stream server
- `circuit_diagram/circuit_schematic.jpeg` — Physical wiring schematic

### Changed
- MQ-2 gas sensor auto-calibration logic: firmware now auto-detects active-high vs active-low board variants
- nRF24L01+ channel changed from default (76) to channel 108 to avoid Wi-Fi interference

### Fixed
- ESP32-CAM GPIO 16 / PSRAM conflict: isolated camera node from all sensors
- Motor brownout on acceleration: separated motor power rail from MCU 5V rail
- Ultrasonic `pulseIn()` blocking: added 30 ms timeout to prevent loop freeze in open space

---

## [v1.0.0] — 2026-05-20 · Initial Prototype Release

### Added
- Initial repository structure
- Basic Arduino sketches for sensor testing (gas, flame, ultrasonic)
- `hardware/Hardware_Specifications.md` — initial component specifications
- `hardware/Cost_Analysis.md` — bill of materials and cost breakdown
- `CONTRIBUTING.md` — initial contribution guidelines
- `LICENSE` — MIT License
- `.gitattributes` — Git LFS configuration for large binary files (images, videos, PDFs)

### Known Issues (v1.0.0)
- ESP32-CAM crashes when GPIO 16 used for sensors (fixed in v1.1.0)
- Motor jitter at joystick neutral — no deadzone applied (fixed in v1.1.0)
- LoRa channel conflicts with nRF on 2.4 GHz (fixed in v1.1.0)

---

## Version Summary

| Version | Date | Status | Highlights |
|:---:|:---:|:---:|:---|
| v1.2.0 | 2026-06-01 | ✅ Latest | Full docs, media, website, community files |
| v1.1.0 | 2026-05-28 | ✅ Stable | Complete firmware, hardware fixes |
| v1.0.0 | 2026-05-20 | 📦 Archived | Initial prototype skeleton |

---

[Unreleased]: https://github.com/Prolayjit-B14/RescueBOT/compare/v1.2.0...HEAD
[v1.2.0]: https://github.com/Prolayjit-B14/RescueBOT/compare/v1.1.0...v1.2.0
[v1.1.0]: https://github.com/Prolayjit-B14/RescueBOT/compare/v1.0.0...v1.1.0
[v1.0.0]: https://github.com/Prolayjit-B14/RescueBOT/releases/tag/v1.0.0
