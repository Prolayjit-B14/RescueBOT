# 📄 Circuit Explanation – RescueBOT

This document explains the physical system architecture, electrical routing, and software logic governing each hardware subsystem of the **RescueBOT** disaster response robot.

---

## 1. System Architecture

The following diagram illustrates the flow of control, sensor telemetry, and video signals between the operator controller, the robot chassis, and the cloud dashboard in the production topology.

```mermaid
graph TD
    %% Base Station / Web Dashboard
    subgraph Base_Station [Base Station & Operations]
        Dashboard["💻 Web Dashboard & Live Stream Viewer"]
        LoRa_RX["📡 ESP32 LoRa RX Base Station (rx-lora.ino)"]
    end

    %% Remote Controller
    subgraph Remote_Controller [Remote Controller (tx.ino)]
        Nano_TX["🤖 Arduino Nano TX Controller"]
        Joy_Car["🕹️ Movement Joystick (A2, A3)"]
        Joy_Arm["🕹️ Robotic Arm Joystick (A0, A1)"]
        Grip_SW["🔘 Gripper Switch (Pin 3)"]
        nRF_TX["📡 nRF24L01+ PA+LNA (Pins 9, 10)"]
    end

    %% Robot Chassis Main
    subgraph Robot_Chassis [Robot Rover Chassis]
        %% Control Subsystem
        subgraph Motor_Arm_Control [Control & Actuation (rx.ino)]
            Nano_RX["🤖 Arduino Nano RX Controller"]
            nRF_RX["📡 nRF24L01+ PA+LNA (Pins 7, 8)"]
            L298N["🔌 L298N Motor Driver (IN1-4, ENA, ENB)"]
            DC_Motors["⚙️ 4x DC Geared BO Motors"]
            Servos["🦾 Robotic Arm Servos (Shoulder, Elbow, Grip, Sweep)"]
        end

        %% Sensor Telemetry Subsystem (LoRa Node)
        subgraph Telemetry_ESP32 [Telemetry Node (tx-lora.ino)]
            ESP_Telemetry["🤖 ESP32 Telemetry Node"]
            GPS_Mod["📍 NEO-6M GPS (UART2 GPIO 16, 17)"]
            MPU_Mod["📐 MPU6050 Gyro/Accel (I2C GPIO 21, 22)"]
            Sensors_Env["🧪 Gas (MQ-2), Vibration (SW-420), Flame (2x KY-026)"]
            Ultra_Sensor["📏 Ultrasonic Sensor (HC-SR04 GPIO 2, 4)"]
            Alarm_Out["🚨 Red/Green LEDs & Active Buzzer (GPIO 12, 14, 15)"]
            LoRa_Mod_TX["📡 SX1278 LoRa Module (SPI GPIO 5, 13, 32)"]
        end

        %% Standalone Surveillance Subsystem
        subgraph Surveillance [Standalone Surveillance Node (production.ino)]
            ESP32_CAM["📷 ESP32-CAM Module"]
            Cam_Sensor["👁️ OV2640 Camera Sensor"]
            Flash_LED["💡 Camera Flash LED (PWM GPIO 4)"]
            WiFi_Cam["📶 Web Video Stream (Port 81)"]
        end
    end

    %% Electrical and Wireless Connections
    Joy_Car --> Nano_TX
    Joy_Arm --> Nano_TX
    Grip_SW --> Nano_TX
    Nano_TX --> nRF_TX

    %% NRF Communication
    nRF_TX -. "Wireless (2.4 GHz NRF)" .-> nRF_RX
    nRF_RX --> Nano_RX
    Nano_RX --> L298N
    L298N --> DC_Motors
    Nano_RX --> Servos

    %% Telemetry Paths (All on one ESP32 via LoRa)
    GPS_Mod --> ESP_Telemetry
    MPU_Mod --> ESP_Telemetry
    Sensors_Env --> ESP_Telemetry
    Ultra_Sensor --> ESP_Telemetry
    ESP_Telemetry --> Alarm_Out
    ESP_Telemetry --> LoRa_Mod_TX

    %% LoRa Telemetry Link
    LoRa_Mod_TX -. "Wireless (433 MHz LoRa)" .-> LoRa_RX
    LoRa_RX -. "Serial bridge" .-> Dashboard

    %% Standalone Video Feed
    Cam_Sensor --> ESP32_CAM
    ESP32_CAM --> Flash_LED
    ESP32_CAM -. "Wi-Fi Video Stream (HTTP Port 81 /stream)" .-> Dashboard
```

---

## 2. Detailed Functional Blocks

### A. RF Control & Actuation Block (nRF24L01 + Joysticks)
The remote control interface manages the steering of the rover and the movements of the robotic arm:
1. **Joystick Input Processing (`tx.ino`):**
   * Two dual-axis analog joysticks read voltages from $0\text{V}$ to $5\text{V}$ (mapped internally to values `0` - `1023`).
   * A smoothing filter ($35\%$ weight of current reading, $65\%$ of previous value) is applied continuously to filter high-frequency noise and mechanical vibrations:
     $$\text{smooth} = (\text{raw} - \text{smooth}) \times 0.35 + \text{smooth}$$
   * Deadzones are established (`90` units for the drive motors, `70` units for the robotic arm) around the central reading of `512` to prevent motor/servo jitter when the sticks are at rest.
2. **RF Packet Transmission:**
   * Packets containing smoothed values are transmitted using an **nRF24L01+ PA+LNA** transceiver module at $250\text{ kbps}$ on **channel 108**. The lower data rate increases receiver sensitivity, extending control range.
3. **Chassis Drive Controller (`rx.ino`):**
   * The receiver parses the drive packet and maps the commands to an **L298N H-Bridge** driver.
   * Forward/Backward and steering directions are handled by setting polarity combinations on pins `IN1` through `IN4`.
   * Motor speed is modulated using PWM signals on the `ENA` and `ENB` enable pins. Standard speed is set to `120` ($47\%$ duty cycle) for forward/backward motions, and lowered to `100` ($39\%$ duty cycle) during turns to ensure high traction.
   * **Fail-Safe Mechanism:** If no RF packets are received within `500 ms` (defined as `TIMEOUT_MS`), the Arduino Nano automatically overrides all controls and halts the motors to prevent runaway situations.
4. **Robotic Arm Control & Panning:**
   * Servos are attached to PWM pins `A2` (Shoulder), `A5` (Elbow), `A4` (Grip), and `A3` (Sweep).
   * Joy offsets increment/decrement joint angles by `5°` steps within hard-coded mechanical limits:
     * **Shoulder:** $0^\circ - 120^\circ$
     * **Elbow:** $0^\circ - 120^\circ$
     * **Grip:** $0^\circ$ (Open) to $90^\circ$ (Closed)
   * **Auto-Panning Sensor:** The `Sweep` servo (pin `A3`) automatically pans the front sensor array continuously between $0^\circ$ and $180^\circ$ in $1^\circ$ steps every $15\text{ ms}$ to sweep the ultrasonic sensor.

### B. Sensory Telemetry Block (LoRa Link)
All environmental sensors are consolidated onto a single ESP32 Node on the rover, transmitting via LoRa to a companion receiver ESP32 at the base:
1. **Vibration Sensor (Analog Input SW-420):**
   * Reads raw analog voltage levels on GPIO `34`.
2. **MQ-2 Gas Sensor (Analog Input):**
   * Reads ambient smoke and gas concentrations on GPIO `27`.
3. **Flame Sensors (Digital Inputs):**
   * Two KY-026 active-low digital infrared flame detectors wired to GPIO `25` and `33`.
4. **MPU6050 Gyroscope/Accelerometer (I2C):**
   * Communicates over I2C (GPIO `21` SDA, GPIO `22` SCL). Accelerometer values calculate Pitch and Roll angles to detect rover tilt/tipover risks.
5. **NEO-6M GPS Module (UART Serial):**
   * Connects via Hardware Serial 2 (GPIO `16` RX2, GPIO `17` TX2) to extract coordinates.
6. **HC-SR04 Ultrasonic (Digital Input/Output):**
   * Measures forward clearance using GPIO `2` (Trig) and GPIO `4` (Echo).
7. **Local Alarm System:**
   * If any sensor exceeds its threshold (Gas $> 1800$, Vibration $> 1500$, Fire detected, or Obstacle $< 20\text{ cm}$), the system enters alert mode, turning on the Red LED (GPIO `12`) and Buzzer (GPIO `15`), while disabling the Green LED (GPIO `14`).
8. **SX1278 LoRa Module (SPI):**
   * Combines all data into a serialized string and broadcasts it via SPI (SS GPIO `5`, RST GPIO `13`, DIO0 GPIO `32`) on the $433\text{ MHz}$ band.
   * **Receiver Node:** A base station ESP32 running `rx-lora.ino` receives this packet via its own SX1278 module (SS GPIO `5`, RST GPIO `14`, DIO0 GPIO `26`) and forwards it to the dashboard.

### C. Standalone ESP32-CAM Surveillance Block
The ESP32-CAM module operates independently from the telemetry systems:
* **Standalone Video Stream:**
  * Runs a dedicated HTTP server on port `81` outputting Motion-JPEG at `/stream`.
  * Leverages internal PSRAM for VGA ($640 \times 480$) resolution or falls back to QVGA if needed.
  * Incorporates flash LED control on GPIO `4` via PWM.
  * Has no physical connection to the environment sensor suite, preventing PSRAM/SPI conflict crashes (GPIO `16`).

---

## 3. Power Architecture

* **Primary Battery Pack:**
  * Rechargeable **18650 Li-ion battery pack** providing $7.4\text{V} - 8.4\text{V}$.
* **Voltage Regulation Rails:**
  * **5V Regulator Rail:** Powers the L298N logic, standard servo motors, buzzer, and HC-SR04 ultrasonic sensors.
  * **3.3V Regulator Rail:** Powers the ESP32 boards, MPU6050, and GPS module.
