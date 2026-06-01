# 📌 Pin Connections – RescueBOT

This document outlines the exact physical pin connections for each microcontroller board in the RescueBOT system based on the active firmware and the production hardware topology.

---

## 🧭 Hardware Topology Summary
1. **Transmitter Remote:** 1x Arduino Nano reads 2 Joysticks + Gripper Switch, transmitting via nRF24L01+.
2. **Receiver Chassis:** 1x Arduino Nano receives nRF24L01+ signals, controlling the L298N motor driver (4x BO Motors) and 4x Robotic Arm servos.
3. **Telemetry Transmitter:** 1x ESP32 Dev Module reads **all sensors** (Vibration, MPU6050, Flame, GPS, Gas, Ultrasonic, Buzzer, LEDs) and transmits them via LoRa.
4. **Telemetry Receiver:** 1x ESP32 Dev Module at the Base Station receives LoRa signals and routes them to the control computer.
5. **Video Feed:** 1x ESP32-CAM runs as a **standalone** surveillance node providing video streaming via Wi-Fi.

---

## 1. Telemetry Transmitter ESP32 Module (`lora_module/tx-lora.ino`)
* **Microcontroller Board:** ESP32 Dev Module / NodeMCU-32S
* **Function:** Main sensor telemetry acquisition, local alert buzzer/LEDs, and long-range LoRa transmission.

| ESP32 Pin | Connected Component | Component Pin | Pin Type | Mode | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **GPIO 34** | SW-420 Vibration Sensor | AOUT | Analog | Input | Structural vibration monitoring |
| **GPIO 27** | MQ-2 Gas & Smoke Sensor | AOUT | Analog | Input | Smoke & hazardous gas detection |
| **GPIO 25** | IR Flame Sensor 1 | DOUT | Digital | Input | Active-low fire sensor 1 |
| **GPIO 33** | IR Flame Sensor 2 | DOUT | Digital | Input | Active-low fire sensor 2 |
| **GPIO 14** | Green Status LED | Positive (+) | Digital | Output | Solid HIGH when status is OPTIMAL (Safe) |
| **GPIO 12** | Red Alert LED | Positive (+) | Digital | Output | Solid HIGH when thresholds are breached (Danger) |
| **GPIO 15** | Active Piezo Buzzer | Positive (+) | Digital | Output | Sounds 2.5 kHz alarm tone when Danger is detected |
| **GPIO 2** | HC-SR04 Ultrasonic Sensor | TRIG | Digital | Output | Distance sensor trigger pulse |
| **GPIO 4** | HC-SR04 Ultrasonic Sensor | ECHO | Digital | Input | Distance sensor echo return pulse |
| **GPIO 21** | MPU6050 Accelerometer | SDA | I2C | I/O | I2C Data line for Pitch/Roll tilt calculation |
| **GPIO 22** | MPU6050 Accelerometer | SCL | I2C | Output | I2C Clock line |
| **GPIO 16** | NEO-6M GPS Module | TXD | UART2 | Input | Hardware Serial RX2 |
| **GPIO 17** | NEO-6M GPS Module | RXD | UART2 | Output | Hardware Serial TX2 |
| **GPIO 5** | SX1278 LoRa Module | NSS / SS | SPI | Output | LoRa Chip Select pin |
| **GPIO 13** | SX1278 LoRa Module | RST | Digital | Output | LoRa Reset control pin |
| **GPIO 32** | SX1278 LoRa Module | DIO0 | Digital | Input | LoRa digital interrupt request line |
| **GPIO 18** | SX1278 LoRa Module | SCK | SPI | Output | SPI Clock line |
| **GPIO 19** | SX1278 LoRa Module | MISO | SPI | Input | SPI MISO line |
| **GPIO 23** | SX1278 LoRa Module | MOSI | SPI | Output | SPI MOSI line |
| **GND** | All Components | GND | Ground | Power | Shared ground rail |

*Note: For the alternative Wi-Fi/MQTT test prototype (`sensors.ino`), the gas sensor MQ2 is wired to **GPIO 35** and the GPS uses UART1. However, the LoRa setup detailed above is the primary telemetry system.*

---

## 2. Telemetry Receiver ESP32 Module (`lora_module/rx-lora.ino`)
* **Microcontroller Board:** ESP32 Dev Module / NodeMCU-32S
* **Function:** Base station receiver for LoRa telemetry broadcasts.

| ESP32 Pin | Connected Component | Component Pin | Pin Type | Mode | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **GPIO 5** | SX1278 LoRa Module | NSS / SS | SPI | Output | LoRa Chip Select pin |
| **GPIO 14** | SX1278 LoRa Module | RST | Digital | Output | LoRa Reset control pin |
| **GPIO 26** | SX1278 LoRa Module | DIO0 | Digital | Input | LoRa interrupt line |
| **GPIO 18** | SX1278 LoRa Module | SCK | SPI | Output | SPI Serial Clock line |
| **GPIO 19** | SX1278 LoRa Module | MISO | SPI | Input | SPI MISO line |
| **GPIO 23** | SX1278 LoRa Module | MOSI | SPI | Output | SPI MOSI line |
| **GND** | LoRa / ESP32 | GND | Ground | Power | Shared ground rail |

---

## 3. Standalone ESP32-CAM Surveillance Module
* **Microcontroller Board:** ESP32-CAM (AI-Thinker)
* **Function:** Standalone live monitoring and camera streaming node over Wi-Fi. It operates independently without any telemetry sensors connected.

### A. Dedicated Camera Sensor Pins (Internal)
| ESP32-CAM Pin | Camera Sensor Connection | Pin Function | Description |
| :--- | :--- | :--- | :--- |
| **GPIO 32** | OV2640 Power Down | PWDN | Camera power state controller |
| **NC (-1)** | OV2640 Reset | RESET | Camera reset pin (Not Connected) |
| **GPIO 0** | OV2640 System Clock | XCLK | Main clock generator |
| **GPIO 26** | OV2640 Serial Data | SIOD | I2C control line data |
| **GPIO 27** | OV2640 Serial Clock | SIOC | I2C control line clock |
| **GPIO 35** | OV2640 Data Bit 9 | Y9 | Video pixel data MSB |
| **GPIO 34** | OV2640 Data Bit 8 | Y8 | Video pixel data |
| **GPIO 39** | OV2640 Data Bit 7 | Y7 | Video pixel data |
| **GPIO 36** | OV2640 Data Bit 6 | Y6 | Video pixel data |
| **GPIO 21** | OV2640 Data Bit 5 | Y5 | Video pixel data |
| **GPIO 19** | OV2640 Data Bit 4 | Y4 | Video pixel data |
| **GPIO 18** | OV2640 Data Bit 3 | Y3 | Video pixel data |
| **GPIO 5** | OV2640 Data Bit 2 | Y2 | Video pixel data LSB |
| **GPIO 25** | OV2640 Vertical Sync | VSYNC | Vertical frame synchronization |
| **GPIO 23** | OV2640 Horiz. Reference | HREF | Horizontal line synchronization |
| **GPIO 22** | OV2640 Pixel Clock | PCLK | Pixel clock synchronization |

### B. General Purpose Peripheral Pins
| ESP32-CAM Pin | Connected Component | Component Pin | Pin Type | Mode | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **GPIO 4** | Camera Flash LED | Positive (+) | PWM | Output | High-power flash LED control (LEDC dimming) |
| **5V / VCC** | Camera Board Power | VCC | Power | Input | 5.0V power input |
| **GND** | Ground | GND | Ground | Power | Shared electrical ground |

---

## 4. Arduino Nano Transmitter (`nrf_communication/tx.ino`)
* **Microcontroller Board:** Arduino Nano V3 (ATmega328P)
* **Function:** Controller remote joystick transmitter.

| Arduino Nano Pin | Connected Component | Component Pin | Pin Type | Mode | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **A0** | Arm Joystick | VRX (X-Axis) | Analog | Input | Robotic arm joint sweeps X-axis controller |
| **A1** | Arm Joystick | VRY (Y-Axis) | Analog | Input | Robotic arm joint sweeps Y-axis controller |
| **A2** | Car Joystick | VRX (X-Axis) | Analog | Input | Chassis movement speed/direction X-axis controller |
| **A3** | Car Joystick | VRY (Y-Axis) | Analog | Input | Chassis movement speed/direction Y-axis controller |
| **Pin 3** | Gripper Toggle Button | OUT / Switch | Digital | Input | Activates gripper open/close (internal pullup) |
| **Pin 9** | nRF24L01+ Transceiver | CE | Digital | Output | Chip Enable transceiver control |
| **Pin 10** | nRF24L01+ Transceiver | CSN | Digital | Output | Chip Select Not (SPI Chip Select) |
| **Pin 11** | nRF24L01+ Transceiver | MOSI | SPI | Output | Master Out Slave In data line |
| **Pin 12** | nRF24L01+ Transceiver | MISO | SPI | Input | Master In Slave Out data line |
| **Pin 13** | nRF24L01+ Transceiver | SCK | SPI | Output | Serial SPI Clock signal |
| **5V** | Joysticks / nRF Power | VCC / +5V | Power | Output | Joysticks VCC and nRF (regulated to 3.3V) |
| **GND** | All Joysticks / Switch / nRF | GND | Ground | Power | Shared ground rail |

---

## 5. Arduino Nano Receiver (`nrf_communication/rx.ino`)
* **Microcontroller Board:** Arduino Nano V3 (ATmega328P)
* **Function:** Drive motor driver and robotic arm servos controller on the rover chassis.

| Arduino Nano Pin | Connected Component | Component Pin | Pin Type | Mode | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Pin 2** | L298N Motor Driver | IN1 | Digital | Output | Left motors forward control direction |
| **Pin 3** | L298N Motor Driver | IN2 | Digital | Output | Left motors backward control direction |
| **Pin 9** | L298N Motor Driver | IN3 | Digital | Output | Right motors backward control direction |
| **Pin 10** | L298N Motor Driver | IN4 | Digital | Output | Right motors forward control direction |
| **Pin 5** | L298N Motor Driver | ENA | PWM | Output | Left motors speed control (analogWrite speed: 100/120) |
| **Pin 6** | L298N Motor Driver | ENB | PWM | Output | Right motors speed control (analogWrite speed: 100/120) |
| **A2** | Shoulder Servo Motor | PWM Signal | PWM | Output | Shoulder joint rotation command line |
| **A3** | Ultrasonic Sweep Servo | PWM Signal | PWM | Output | Sweep joint rotation command line (panning sensor) |
| **A4** | Gripper Servo Motor | PWM Signal | PWM | Output | Claw opening and closing control |
| **A5** | Elbow Servo Motor | PWM Signal | PWM | Output | Elbow joint vertical bend command line |
| **Pin 7** | nRF24L01+ Transceiver | CE | Digital | Output | Chip Enable transceiver control |
| **Pin 8** | nRF24L01+ Transceiver | CSN | Digital | Output | Chip Select Not (SPI Chip Select) |
| **Pin 11** | nRF24L01+ Transceiver | MOSI | SPI | Input | SPI Data from Master to Transceiver |
| **Pin 12** | nRF24L01+ Transceiver | MISO | SPI | Output | SPI Data from Transceiver to Master |
| **Pin 13** | nRF24L01+ Transceiver | SCK | SPI | Input | SPI Clock signal from Arduino Nano |
| **5V** | Servos VCC (from regulator) | VCC / Red | Power | Output | Servos 5V power supply (drawn from separate external regulator) |
| **GND** | Driver, Servos, nRF | GND | Ground | Power | Shared system common ground |
