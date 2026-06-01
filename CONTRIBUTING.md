# 🤝 Contributing to RescueBOT

Thank you for your interest in contributing to **RescueBOT**! This project is an intelligent disaster response robotic platform designed to assist in hazardous environments. Contributions from the open-source community are what make projects like this thrive.

Please take a moment to review this guide before submitting issues, proposing hardware revisions, or writing firmware code.

---

## 🚀 How to Contribute

### 1. Reporting Bugs & Hardware Issues
* Search the existing issues/documentation before creating a new report.
* Be clear and concise.
* When reporting firmware bugs, include:
  * The specific board used (e.g., ESP32-CAM, ESP32 Dev Module, Arduino Nano).
  * System serial output logs.
  * Step-by-step instructions to reproduce the crash.
* When reporting hardware issues, include:
  * Schematics or wiring references.
  * Power distribution notes (voltage drop, regulator heat, etc.).

### 2. Proposing Features & Hardware Upgrades
If you want to add a new sensor, optimize the communication link, or design a new robotic arm joint:
1. Open an issue explaining the **Purpose** of the new feature.
2. Outline the **Hardware Requirements** (e.g., pin budget, current draw, voltage rails).
3. Discuss the **Software Impact** (e.g., changes to telemetry payload schemas).

### 3. Submission Workflow
1. **Fork** the repository and create your feature branch:
   ```bash
   git checkout -b feature/amazing-feature
   ```
2. **Commit** your changes with clear, descriptive commit messages.
3. **Push** to your branch:
   ```bash
   git push origin feature/amazing-feature
   ```
4. Open a **Pull Request** (PR) detailing the modifications and explaining how you verified them.

---

## 💻 Firmware Coding Guidelines

Our firmware is written in C++ for Arduino and ESP32 environments. To maintain a clean and reliable codebase, follow these rules:

### A. Code Style & Structure
* **Pin Definitions**: Define all hardware pins at the top of the file using uppercase macros (e.g., `#define VIBRATION_PIN 34`).
* **Pin Conflicts**: Always check [pin_connections.md](file:///c:/Users/polu1/OneDrive/Documents/Desktop/RescueBOT/circuit_diagram/pin_connections.md) before assigning pins to ensure you do not create a conflict on the target board (such as reusing GPIO 26 for different sensors/lines).
* **Comments**: Include clear, inline comments for hardware configuration setup registers, complex math formulas, or non-obvious logic.

### B. Non-Blocking Execution (Crucial)
Disaster response robots must run in real-time without locking up.
* **Avoid `delay()`**: Never use blocking delays inside main `loop()` functions. Instead, implement timing intervals using `millis()` loops:
  ```cpp
  unsigned long now = millis();
  if (now - lastUpdate > updateInterval) {
    lastUpdate = now;
    // Execute periodic action
  }
  ```
* **Guard IO Timeouts**: When using functions that poll pins, always set explicit timeouts:
  * Use `pulseIn(ECHO_PIN, HIGH, 30000)` (30ms timeout) to prevent blocking if the ultrasonic echo is missed.
* **Fail-safes**: Implement communication timeouts. For example, motors must automatically stop if control signals (such as NRF packets) are lost for more than `500 ms`.

### C. ESP32-CAM Constraints
* **PSRAM Conflicts**: Avoid using GPIO `16` (camera clock pin) for peripheral sensors when PSRAM is enabled, as it will corrupt memory and crash the microcontroller.
* **Core Version Macros**: Support multiple core releases. Check version differences using prepressor macros (e.g., `#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)`).

---

## 📐 Hardware & Documentation Standards

Any changes to hardware components or physical circuits **must** be documented immediately to keep the schematics and code aligned.

### 1. Pin Configuration updates
If you change a physical pin wiring or assign a new sensor pin:
* Update the appropriate board tables in [pin_connections.md](file:///c:/Users/polu1/OneDrive/Documents/Desktop/RescueBOT/circuit_diagram/pin_connections.md).
* Update the functional block descriptions in [circuit_explanation.md](file:///c:/Users/polu1/OneDrive/Documents/Desktop/RescueBOT/circuit_diagram/circuit_explanation.md).
* Update the system architecture Mermaid diagrams if necessary.

### 2. Hardware Specs & Bill of Materials (BOM)
* If adding/removing components, update the item list in [Component_List.md](file:///c:/Users/polu1/OneDrive/Documents/Desktop/RescueBOT/hardware/Component_List.md) and the technical parameters in [Hardware_Specifications.md](file:///c:/Users/polu1/OneDrive/Documents/Desktop/RescueBOT/hardware/Hardware_Specifications.md).
* Keep pricing references up-to-date in [Cost_Analysis.md](file:///c:/Users/polu1/OneDrive/Documents/Desktop/RescueBOT/hardware/Cost_Analysis.md) to help future builders estimate budget requirements.

---

## 🏁 Verification Checklist

Before pushing a PR:
- [ ] Code compiles without errors on target boards in Arduino IDE / VS Code.
- [ ] No duplicate GPIO pins are assigned on the same board.
- [ ] The robot enters a safe standby state (motors stopped, alarms disabled) on boot.
- [ ] Telemetry JSON outputs conform to the dashboard specifications.
- [ ] All related documentation files (`pin_connections.md`, `Hardware_Specifications.md`) are updated and correct.
