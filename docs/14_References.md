# 📄 14 — References: Datasheets, Research Literature & Technical Documentation

---

## 1. Component Datasheets & Official Technical Documentation

### 1.1 Microcontrollers & Processing Boards

| Component | Document | Source |
|:---|:---|:---|
| **ESP32-WROOM-32** | ESP32-WROOM-32 Datasheet (v3.4) | Espressif Systems — [espressif.com](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf) |
| **ESP32 Technical Reference Manual** | ESP32 TRM (v5.3) | Espressif Systems — [espressif.com](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf) |
| **ESP32-CAM (AI-Thinker)** | ESP32-CAM Schematic & Product Specification | AI-Thinker — GitHub ESP32-CAM repositories |
| **OV2640 Camera Sensor** | OV2640 Datasheet (UXGA) | OmniVision Technologies — available via Espressif esp32-camera GitHub |
| **Arduino Nano** | ATmega328P Datasheet (8-bit AVR) | Microchip Technology — [microchip.com](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf) |

### 1.2 RF Transceivers

| Component | Document | Source |
|:---|:---|:---|
| **nRF24L01+** | nRF24L01+ Product Specification v1.0 | Nordic Semiconductor — [nordicsemi.com](https://www.nordicsemi.com/Products/nRF24L01) |
| **SX1278 LoRa** | SX1276/77/78/79 Datasheet (DS.SX1276-7-8-9_W_APP_V7) | Semtech Corporation — [semtech.com](https://www.semtech.com/products/wireless-rf/lora-connect/sx1276) |
| **SX1278 RA-02 Module** | Ai-Thinker RA-02 Module Specification | Ai-Thinker Technology — available via product distributor datasheets |

### 1.3 Environmental Sensors

| Component | Document | Source |
|:---|:---|:---|
| **MQ-2 Gas Sensor** | MQ-2 Sensitive Material Datasheet | Hanwei Electronics (Zhengzhou) — widely distributed PDF |
| **SW-420 Vibration Sensor** | SW-420 Vibration Switch Module Datasheet | Generic manufacturer spec sheet — available via electronics distributors |
| **KY-026 Flame Sensor** | KY-026 IR Flame Detection Module Spec | Generic sensor module documentation |
| **HC-SR04 Ultrasonic** | HC-SR04 Ultrasonic Distance Measurement Module User Guide | Various manufacturers — [elecfreaks.com](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf) |
| **MPU-6050** | MPU-6000 and MPU-6050 Product Specification Rev 3.4 | InvenSense (TDK) — [invensense.tdk.com](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf) |
| **MPU-6050 Register Map** | MPU-6000/MPU-6050 Register Map and Descriptions Rev 4.2 | InvenSense (TDK) — [invensense.tdk.com](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf) |
| **NEO-6M GPS** | u-blox NEO-6 u-blox 6 GPS Modules Data Sheet | u-blox AG — [u-blox.com](https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf) |
| **NEO-6M Protocol** | u-blox 6 Receiver Description Including Protocol Specification | u-blox AG — [u-blox.com](https://www.u-blox.com) |

### 1.4 Actuators & Power Components

| Component | Document | Source |
|:---|:---|:---|
| **L298N Motor Driver** | L298N Dual Full-Bridge Driver Datasheet | STMicroelectronics — [st.com](https://www.st.com/resource/en/datasheet/l298.pdf) |
| **SG90 Micro Servo** | SG90 9g Micro Servo Motor Datasheet | Tower Pro — widely distributed spec sheet |
| **MG90S Metal Gear Servo** | MG90S Datasheet | Tower Pro — widely distributed spec sheet |
| **SSD1306 OLED** | SSD1306 Advance Information Datasheet | Solomon Systech — [digikey.com resource](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf) |

---

## 2. Software Libraries & Firmware References

| Library | Author / Maintainer | Repository / Documentation |
|:---|:---|:---|
| **RF24 (nRF24L01+ Driver)** | TMRh20, Avamander | [github.com/nRF24/RF24](https://github.com/nRF24/RF24) |
| **LoRa (SX1278 Driver)** | Sandeep Mistry | [github.com/sandeepmistry/arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa) |
| **TinyGPSPlus** | Mikal Hart | [github.com/mikalhart/TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) |
| **Adafruit MPU6050** | Adafruit Industries | [github.com/adafruit/Adafruit_MPU6050](https://github.com/adafruit/Adafruit_MPU6050) |
| **Adafruit Unified Sensor** | Adafruit Industries | [github.com/adafruit/Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor) |
| **Adafruit SSD1306** | Adafruit Industries | [github.com/adafruit/Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) |
| **ArduinoJson** | Benoit Blanchon | [arduinojson.org](https://arduinojson.org) |
| **esp32-camera** | Espressif Systems | [github.com/espressif/esp32-camera](https://github.com/espressif/esp32-camera) |
| **ESP32 Arduino Core** | Espressif Systems | [github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32) |

---

## 3. Research Literature — Disaster Robotics & Related Fields

### [1] Murphy, R. R. (2014). *Disaster Robotics.* MIT Press.
> Comprehensive reference on the use of robotic systems in disaster response, covering teleoperation, autonomy, human-robot interaction, and field deployment case studies including 9/11, Hurricane Katrina, and Fukushima Daiichi nuclear disaster.

### [2] Murphy, R. R., Tadokoro, S., Nardi, D., Jacoff, A., Fiorini, P., Choset, H., & Erkmen, A. M. (2008). *Search and Rescue Robotics.* In Springer Handbook of Robotics.
> Landmark chapter covering UGV and UAV search-and-rescue systems, sensor integration requirements, and human-in-the-loop teleoperation architectures.

### [3] Queralta, J. P., Almansa-Valverde, J., Schiano, F., Floreano, D., Westerlund, T., & Heikkonen, J. (2020). *Collaborative Multi-Robot Search and Rescue: Planning, Communication and Coordination.* IEEE Access, 8, 191617–191643.
> DOI: 10.1109/ACCESS.2020.3030146
> Provides theoretical and experimental validation of multi-robot coordination approaches for disaster scenarios, relevant to RescueBOT's Phase 4 swarm robotics roadmap.

### [4] Erturk, M. A., Aydin, M. A., Buyukakkaşlar, M. T., & Evirgen, H. (2019). *A Survey on LoRa Architecture, Protocol and Technologies.* Future Internet, 11(10), 216.
> DOI: 10.3390/fi11100216
> Reviews LoRa/LoRaWAN architecture, physical layer characteristics, and propagation models — foundational for understanding RescueBOT's 433 MHz telemetry design choices.

### [5] Poole, M., Poole, O., & Poole, K. (2019). *LoRa propagation through concrete and reinforced concrete.* 2019 IEEE Radio and Wireless Symposium (RWS).
> Empirical measurements of LoRa signal penetration through concrete walls, directly supporting RescueBOT's claim of multi-wall telemetry capability.

### [6] Xia, F., Yang, L. T., Wang, L., & Vinel, A. (2012). *Internet of Things.* International Journal of Communication Systems, 25(9), 1101–1102.
> Context reference for IoT architecture patterns applicable to RescueBOT's telemetry pipeline and future cloud integration design.

### [7] Nagatani, K., Kiribayashi, S., Okada, Y., Otake, K., Yoshida, K., Tadokoro, S., ... & Kimura, S. (2013). *Emergency response to the nuclear accident at the Fukushima Daiichi Nuclear Power Plants using mobile rescue robots.* Journal of Field Robotics, 30(1), 44–63.
> DOI: 10.1002/rob.21439
> Real-world case study of disaster robots deployed at Fukushima, documenting communication challenges, radiation effects, and operational lessons directly relevant to RescueBOT's design priorities.

### [8] FEMA (2022). *National Urban Search and Rescue Response System: Rescue Field Operations Guide.* Federal Emergency Management Agency.
> Operational procedures reference for understanding what data (gas levels, structural stability, survivor detection) rescue teams actually require from first-response scouts — used to validate RescueBOT's sensor selection.

---

## 4. Development Platform Documentation

| Platform | Document | URL |
|:---|:---|:---|
| **Arduino Platform** | Arduino Language Reference | [arduino.cc/reference](https://www.arduino.cc/reference/en/) |
| **ESP32 Arduino Core** | ESP32 Arduino Core Documentation | [docs.espressif.com](https://docs.espressif.com/projects/arduino-esp32/en/latest/) |
| **Espressif ESP32** | ESP-IDF Programming Guide | [docs.espressif.com/projects/esp-idf](https://docs.espressif.com/projects/esp-idf/en/latest/) |
| **Leaflet.js** | Leaflet.js Documentation v1.9 | [leafletjs.com](https://leafletjs.com/reference.html) |
| **Google Maps JS API** | Google Maps Platform Documentation | [developers.google.com/maps](https://developers.google.com/maps/documentation/javascript) |
| **NMEA 0183** | NMEA 0183 Standard (v4.11) | National Marine Electronics Association |

---

## 5. Project-Internal References

| Document | Path | Description |
|:---|:---|:---|
| Circuit Explanation | [`circuit_diagram/circuit_explanation.md`](../circuit_diagram/circuit_explanation.md) | Complete system wiring, functional block descriptions, and power architecture |
| Pin Connections | [`circuit_diagram/pin_connections.md`](../circuit_diagram/pin_connections.md) | Full pin mapping tables for all 5 microcontroller boards |
| LoRa TX Firmware | [`firmware/lora_module/tx-lora.ino`](../firmware/lora_module/tx-lora.ino) | Telemetry ESP32 main firmware source |
| LoRa RX Firmware | [`firmware/lora_module/rx-lora.ino`](../firmware/lora_module/rx-lora.ino) | Base station receiver firmware source |
| nRF TX Firmware | [`firmware/nrf_communication/tx.ino`](../firmware/nrf_communication/tx.ino) | Remote controller transmitter firmware |
| nRF RX Firmware | [`firmware/nrf_communication/rx.ino`](../firmware/nrf_communication/rx.ino) | Chassis motor/servo controller firmware |
| Camera Firmware | [`firmware/cam_module/production.ino`](../firmware/cam_module/production.ino) | ESP32-CAM MJPEG stream server firmware |
| Circuit Schematic | [`circuit_diagram/circuit_schematic.jpeg`](../circuit_diagram/circuit_schematic.jpeg) | Physical wiring schematic diagram |
| Component List | [`media/components_list.png`](../media/components_list.png) | Full hardware component inventory |
| Cost Analysis | [`media/cost_analysis.png`](../media/cost_analysis.png) | Bill of Materials cost breakdown |
| Hardware Specs | [`media/hardware_spesification.png`](../media/hardware_spesification.png) | Hardware specification reference image |
| Pin Connection Diagram | [`media/pin_connection.png`](../media/pin_connection.png) | Visual pin connection reference |

---

*Previous: [13 — Team Details ←](./13_Team_Details.md)*

---

> **Citation Note:** Where specific academic paper DOIs are provided, references follow IEEE citation style. For datasheets and technical documentation, manufacturer official sources are cited. All URLs were verified accessible as of June 2026.
