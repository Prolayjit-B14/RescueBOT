# 🗺️ ROADMAP — RescueBOT Development Plan

> **Current Version:** v1.2.0 (ZYRO 2026 Prototype)
> **Maintained by:** Team BOT THINGS | [@Prolayjit-B14](https://github.com/Prolayjit-B14)

This roadmap outlines the planned evolution of RescueBOT from hackathon prototype to a field-deployable, AI-powered, swarm-capable disaster response platform.

---

## Status Legend

| Symbol | Meaning |
|:---:|:---|
| ✅ | **Done** — implemented and tested |
| 🔄 | **In Progress** — actively being worked on |
| 📋 | **Planned** — defined, not yet started |
| 💡 | **Research** — concept stage, under evaluation |

---

## ✅ Phase 0 — Foundation (Completed: May 2026)

> *Proof-of-concept hardware + firmware validated at ZYRO 2026*

- ✅ 4-wheel differential drive UGV chassis assembly
- ✅ nRF24L01+ PA+LNA dual-joystick remote control (Channel 108, 250 kbps)
- ✅ ESP32 multi-sensor telemetry node (MQ-2, SW-420, KY-026×2, HC-SR04, MPU6050, NEO-6M)
- ✅ SX1278 LoRa 433 MHz sensor telemetry uplink (350 m urban / 1.2 km LOS)
- ✅ Standalone ESP32-CAM MJPEG video stream (VGA ~22 FPS, Port 81)
- ✅ 4DOF robotic gripper arm (MG90S × 2 + SG90 × 2, 100–150g payload)
- ✅ Local hardware alarm (Red LED + 2500 Hz buzzer)
- ✅ Drive fail-safe (500 ms nRF timeout → auto-halt)
- ✅ Web dashboard (sensors, GPS map, camera, alert panel)
- ✅ Full 14-file technical documentation suite
- ✅ Professional GitHub repository with community files

---

## 🔄 Phase 1 — Sensor Completion (Q3 2026)

> *Activating hardware already on the BOM but not yet in firmware*

- 📋 **PIR Human Presence Detection** — Integrate HC-SR501 PIR into telemetry firmware; add `PIR=1/0` to LoRa packet; dedicated "survivor detected" alert on dashboard
- 📋 **OLED Local Display** — Activate SSD1306 0.96" display on shared I2C bus; rotating 4-page display (Gas/Vib → Flame/Dist → GPS → IMU)
- 📋 **Relay Payload Drop** — Relay module on Arduino Nano RX; controlled via gripper toggle extension; one-shot payload deployment without arm repositioning
- 📋 **MQ-2 PPM Calibration Curve** — Implement lookup table for approximate PPM output from ADC voltage ratio (requires datasheet R₀ calibration in clean air)
- 📋 **LoRa Packet AES-128 Encryption** — Encrypt telemetry payload using shared AES-128 key; prevents unauthorized packet interception on 433 MHz band

---

## 📋 Phase 2 — Autonomous Navigation (Q4 2026)

> *Removing the need for continuous manual joystick control*

- 📋 **GPS Waypoint Navigation** — Operator inputs target lat/lon on dashboard; UGV calculates bearing (haversine formula) and autonomously drives to coordinates with ±3 m tolerance
- 📋 **360° Ultrasonic Array** — Add 3 additional HC-SR04 sensors (rear, left, right) for full static obstacle awareness without sweep servo dependency
- 📋 **Wheel Odometry (Dead Reckoning)** — Add encoder discs to BO motor axles; count pulses for distance estimation in GPS-denied indoor environments
- 💡 **2D LiDAR + SLAM** — Integrate RPLiDAR A1 or LD19 on a Raspberry Pi Zero 2W; run GMapping or Cartographer for 2D occupancy grid mapping; stream map to dashboard over LoRa delta packets

---

## 📋 Phase 3 — Edge AI & Computer Vision (Q1 2027)

> *Making the rover smarter — detect humans, classify hazards*

- 📋 **Human Silhouette Detection** — Deploy MobileNet-SSD V2 INT8 on Raspberry Pi Zero 2W or ESP32-S3; detect human class from OV2640 video feed; `HUMAN_DETECTED` LoRa alert with bounding box metadata
- 📋 **Thermal Camera Overlay** — Integrate MLX90640 32×24 thermal sensor (I2C, 7 Hz); overlay thermal gradient onto OV2640 MJPEG feed using OpenCV; highlight heat signatures (body temp 36–37°C) in orange/red
- 📋 **Multi-Gas Classification** — Expand to MQ-4 (CH₄), MQ-7 (CO), MQ-135 (NH₃) array; train a small classifier on multi-sensor voltage ratios to identify specific gas type, not just concentration
- 💡 **Fire Hotspot Mapping** — Fuse GPS + thermal data to generate a 2D heat map overlay on the dashboard map showing fire boundaries

---

## 📋 Phase 4 — Aerial Integration & Swarm (Q2 2027)

> *Expanding from single UGV to ground-air coordinated swarm*

- 📋 **Drone Docking Platform** — Retractable landing pad on UGV top deck; Qi 5W wireless charging; alignment guides for autonomous landing
- 📋 **LoRa-to-MAVLink Bridge** — Route drone GPS, attitude, and battery telemetry through UGV's LoRa link back to base station; unified dashboard for both ground and aerial units
- 📋 **Coordinated Search** — Drone dispatched from UGV for aerial rubble field mapping; thermal camera on drone identifies survivor locations; coordinates sent to UGV for precise navigation
- 📋 **Multi-Robot Swarm (3–10 units)** — Unique LoRa address per unit; base station aggregates all telemetry; units broadcast GPS positions to coordinate search zones; swarm coverage cost < ₹50,000 for 5 units
- 💡 **Swarm Path Coordination** — Shared occupancy grid across units over LoRa; automatic zone assignment to prevent search overlap

---

## 📋 Phase 5 — Cloud & Production Hardening (Q3 2027)

> *From field prototype to deployable product*

- 📋 **Cloud Telemetry Pipeline** — AWS IoT Core + DynamoDB + Grafana dashboard; MQTT broker for multi-unit data aggregation; share telemetry with international relief agencies in real time
- 📋 **Mobile App (Android / iOS)** — React Native cross-platform app; real-time sensor gauges, MJPEG stream, GPS map, virtual joystick; push notifications on `ALERT=1`
- 📋 **ESP32 Secure Boot + OTA** — Enable ESP32 secure boot with signed firmware; OTA update via HTTPS without physical USB access; rolling firmware keys
- 📋 **IP44 Weatherproof Enclosure** — Aluminum or PETG chassis; conformal-coated PCBs; rubber-gasketed sensor apertures; rated for rain and dust ingress protection
- 📋 **nRF Rolling-Code Authentication** — HMAC-signed control packets; replay attack prevention for the nRF24L01+ control link
- 💡 **Regulatory Compliance** — CE/FCC certification pathway for 433 MHz and 2.4 GHz RF modules for commercial deployment

---

## 📊 Roadmap Summary Timeline

```
2026 Q2  ████████████████████  Phase 0 (DONE) ✅
2026 Q3  ████████████░░░░░░░░  Phase 1 — Sensor Completion
2026 Q4  ░░░░░░████████░░░░░░  Phase 2 — Autonomous Navigation
2027 Q1  ░░░░░░░░░░████████░░  Phase 3 — Edge AI & Computer Vision
2027 Q2  ░░░░░░░░░░░░░░████████Phase 4 — Aerial & Swarm
2027 Q3  ░░░░░░░░░░░░░░░░░░████Phase 5 — Cloud & Production
```

---

## 💬 Contributing to the Roadmap

Have an idea that should be on this roadmap?

- 💡 **Suggest a feature** → [Open a Discussion](https://github.com/Prolayjit-B14/RescueBOT/discussions)
- 🐛 **Report a bug** → [Open an Issue](https://github.com/Prolayjit-B14/RescueBOT/issues)
- 🔧 **Implement a roadmap item** → [Read CONTRIBUTING.md](CONTRIBUTING.md) and open a PR

---

*Last Updated: June 2026 | Maintained by Team BOT THINGS*
