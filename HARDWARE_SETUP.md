# 🔧 HARDWARE SETUP — RescueBOT Assembly & Wiring Guide

> **Reference documents:** [`circuit_diagram/pin_connections.md`](circuit_diagram/pin_connections.md) · [`circuit_diagram/circuit_explanation.md`](circuit_diagram/circuit_explanation.md)

---

## Bill of Materials (Complete BOM)

| # | Component | Model | Qty | Est. Cost (₹) |
|:---:|:---|:---|:---:|:---:|
| 1 | ESP32 DevKit | ESP32-WROOM-32 | 2 | ₹480 |
| 2 | ESP32-CAM | AI-Thinker (OV2640) | 1 | ₹380 |
| 3 | Arduino Nano | V3.0 ATmega328P | 2 | ₹300 |
| 4 | nRF24L01+ PA+LNA | With SMA antenna | 2 | ₹320 |
| 5 | SX1278 LoRa Module | RA-02 433 MHz | 2 | ₹700 |
| 6 | MQ-2 Gas Sensor | With comparator board | 1 | ₹120 |
| 7 | SW-420 Vibration Sensor | Module with POT | 1 | ₹60 |
| 8 | KY-026 Flame Sensor | IR phototransistor | 2 | ₹80 |
| 9 | HC-SR04 Ultrasonic | Distance sensor | 1 | ₹60 |
| 10 | MPU6050 IMU | GY-521 module | 1 | ₹120 |
| 11 | NEO-6M GPS | With ceramic antenna | 1 | ₹450 |
| 12 | L298N Motor Driver | Dual H-Bridge module | 1 | ₹120 |
| 13 | BO Geared DC Motor | Plastic gear, 3–6V | 4 | ₹200 |
| 14 | MG90S Metal Servo | Metal gear | 2 | ₹280 |
| 15 | SG90 Micro Servo | Plastic gear | 2 | ₹140 |
| 16 | SSD1306 OLED | 0.96" I2C | 1 | ₹120 |
| 17 | Active Buzzer | 5V piezo | 1 | ₹20 |
| 18 | LED (Red + Green) | 5mm | 2 | ₹10 |
| 19 | 18650 Li-ion Battery | 3.7V 2500 mAh | 2 | ₹300 |
| 20 | 18650 Battery Holder | 2S series | 1 | ₹40 |
| 21 | Buck Converter | LM2596 5V 3A | 1 | ₹80 |
| 22 | 4WD Acrylic Chassis | With wheels & mounts | 1 | ₹350 |
| 23 | Robotic Arm Kit | 4DOF servo bracket set | 1 | ₹450 |
| 24 | Jumper Wires | M-M, M-F, F-F sets | 1 set | ₹80 |
| 25 | USB-TTL Adapter | CH340G (for CAM flash) | 1 | ₹80 |
| 26 | Misc (resistors, caps, PCB) | — | — | ₹100 |
| | | **TOTAL** | | **≈ ₹5,590** |

> *Prices are approximate Indian market rates as of 2026. Total BOM well under ₹10,000 including shipping.*

---

## Power Architecture

```
2S Li-ion Pack (7.4V – 8.4V)
│
├──[Direct] ──────────────────► L298N Motor VM → 4× BO DC Motors
│
├──[LM2596 Buck → 5V 3A] ────► Arduino Nano RX + TX (5V pin)
│                              ► MG90S × 2 + SG90 × 2 Servos
│                              ► HC-SR04 VCC
│                              ► Active Buzzer
│                              ► L298N Logic VSS
│
├──[ESP32 AMS1117 → 3.3V] ───► MPU6050 VCC (via ESP32 3V3 pin)
│                              ► NEO-6M GPS VCC
│                              ► SX1278 LoRa VCC
│                              ► KY-026 × 2 VCC
│                              ► SW-420 VCC
│
└──[Separate 5V USB / Regulator] ► ESP32-CAM VCC (MUST be 5V)
```

> ⚠️ **Critical:** ESP32-CAM needs its own 5V supply. Do **not** power it from the 3.3V rail — it will fail to boot or crash during Wi-Fi TX.

---

## Chassis Assembly Steps

### Step 1 — Bottom Plate
1. Mount 4× BO motors (2 per side) using M3 bolts through chassis slots.
2. Attach wheels to motor shafts.
3. Mount L298N motor driver centrally between motor pairs.
4. Secure 18650 battery holder at the centre for weight balance.
5. Wire motor pairs: left motors → L298N OUT1/OUT2; right motors → OUT3/OUT4.

### Step 2 — Upper Plate
1. Mount Arduino Nano RX near the motor driver (short wire runs).
2. Mount Telemetry ESP32 near the front-centre.
3. Mount SX1278 LoRa module adjacent to ESP32 (SPI wires kept short).
4. Mount LM2596 5V buck converter; input from battery, output to 5V rail.
5. Route a **common ground bus** — connect ALL board GNDs to one point.

### Step 3 — Front Sensor Bar
1. Mount SG90 sweep servo on a front bracket facing forward.
2. Attach HC-SR04 ultrasonic to the sweep servo output arm.
3. Mount KY-026 Flame Sensor × 2 symmetrically, angled slightly outward (~15° each).
4. All sensor cables route up through the chassis to the ESP32 on the upper plate.

### Step 4 — Robotic Arm
1. Base-mount Shoulder MG90S servo to the front of the upper plate.
2. Attach Elbow MG90S to the shoulder output shaft bracket.
3. Attach Gripper SG90 at the forearm terminal end.
4. Route servo signal wires along the arm structure — use cable ties every joint to prevent wire tension during movement.

### Step 5 — Sensor Placement
| Sensor | Mount Location | Notes |
|:---|:---|:---|
| MQ-2 Gas | Upper deck, front-left | Away from motor heat |
| SW-420 Vibration | Lower chassis base plate | Direct contact with frame |
| MPU6050 IMU | Upper deck, flat & level | Level mounting critical |
| NEO-6M GPS | Top-most position | Antenna facing skyward |
| ESP32-CAM | Front bracket, forward-facing | Protected lens housing recommended |

---

## Wiring Summary (Key Connections)

### ESP32 Telemetry Node
| GPIO | Component | Pin |
|:---:|:---|:---:|
| 27 | MQ-2 Analog Out | AOUT |
| 34 | SW-420 Analog Out | AOUT |
| 25 | KY-026 #1 Digital | DOUT |
| 33 | KY-026 #2 Digital | DOUT |
| 2 | HC-SR04 | TRIG |
| 4 | HC-SR04 | ECHO |
| 21 | MPU6050 | SDA |
| 22 | MPU6050 | SCL |
| 16 | NEO-6M | TX → ESP32 RX2 |
| 17 | NEO-6M | RX → ESP32 TX2 |
| 5 | SX1278 LoRa | NSS |
| 13 | SX1278 LoRa | RST |
| 32 | SX1278 LoRa | DIO0 |
| 18/19/23 | SX1278 LoRa | SCK/MISO/MOSI |
| 12 | Red LED | + (330Ω series) |
| 14 | Green LED | + (330Ω series) |
| 15 | Buzzer | + |

### Arduino Nano RX (Chassis Controller)
| Pin | Component | Connection |
|:---:|:---|:---|
| 2 | L298N | IN1 |
| 3 | L298N | IN2 |
| 9 | L298N | IN3 |
| 10 | L298N | IN4 |
| 5 (PWM) | L298N | ENA |
| 6 (PWM) | L298N | ENB |
| A2 (PWM) | Shoulder Servo | Signal |
| A5 (PWM) | Elbow Servo | Signal |
| A4 (PWM) | Gripper Servo | Signal |
| A3 (PWM) | Sweep Servo | Signal |
| 7 | nRF24L01+ | CE |
| 8 | nRF24L01+ | CSN |
| 11/12/13 | nRF24L01+ | MOSI/MISO/SCK |

> **Full pin tables for all 5 boards:** [`circuit_diagram/pin_connections.md`](circuit_diagram/pin_connections.md)

---

## I2C Pull-Up Resistors

The I2C bus (GPIO 21/22) shared by MPU6050 and SSD1306 OLED requires pull-up resistors:

```
ESP32 GPIO 21 (SDA) ──[4.7 kΩ to 3.3V]──┬── MPU6050 SDA
                                           └── SSD1306 SDA
ESP32 GPIO 22 (SCL) ──[4.7 kΩ to 3.3V]──┬── MPU6050 SCL
                                           └── SSD1306 SCL
```

Without these resistors, I2C communication becomes unreliable above 100 kHz.

---

## Decoupling Capacitors

Add these to reduce motor-noise-induced sensor reading errors:

| Location | Capacitor | Value |
|:---|:---|:---:|
| MQ-2 VCC pin | Ceramic | 100 nF |
| SW-420 VCC pin | Ceramic | 100 nF |
| 5V buck converter output | Electrolytic | 10 µF |
| nRF24L01+ VCC (3.3V) | Electrolytic | 10 µF |

---

## Safety Checklist Before Power-On

- [ ] All ground connections tied to a single common bus point
- [ ] No GPIO 16 used on ESP32-CAM for any external component
- [ ] nRF24L01+ powered from 3.3V (NOT 5V — will damage module)
- [ ] ESP32-CAM powered from 5V (NOT 3.3V — will fail to boot)
- [ ] Motor power (battery) isolated from MCU power (buck converter)
- [ ] No short circuits between 5V servo rail and 3.3V sensor rail
- [ ] Servo signal wires confirmed on correct Arduino Nano PWM pins

---

*See also: [circuit_diagram/circuit_explanation.md](circuit_diagram/circuit_explanation.md) for detailed functional block descriptions.*
