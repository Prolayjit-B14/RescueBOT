# 🤝 Contributing to RescueBOT

Thank you for your interest in contributing to the **RescueBOT** project! Whether you are a fellow B.Tech student, a robotics enthusiast, or an open-source contributor, your help is welcome.

## How Can I Contribute?

### 1. Reporting Bugs
If you build the project and find a flaw in the code or the circuit design, please open an Issue on GitHub.
* Be sure to include your hardware setup.
* Provide serial monitor logs if the ESP32 is crashing.
* Provide browser console logs if the Web Dashboard is failing.

### 2. Suggesting Enhancements
Have an idea to make RescueBOT better? We'd love to hear it. (e.g., "Add a thermal camera support", "Write a Python backend instead of using public MQTT"). Open an Issue with the tag `enhancement`.

### 3. Code Contributions
If you want to write code, please follow this workflow:

1. **Fork the Repository:** Create your own copy of the project.
2. **Create a Branch:** `git checkout -b feature/AmazingNewFeature`
3. **Commit your Changes:** Write clear, concise commit messages. `git commit -m 'Add support for RPLidar'`
4. **Push to the Branch:** `git push origin feature/AmazingNewFeature`
5. **Open a Pull Request:** Go to the original repository and click "New Pull Request".

## Areas Where We Need Help
As this is primarily a student project, we are actively looking for contributors to help with:

* **PCB Design:** Converting our messy breadboard diagrams into clean Gerber files for custom PCBs.
* **3D Printing:** Designing custom enclosures for the ESP32 and the sensors to make them waterproof/dustproof.
* **Autonomous Navigation:** Integrating ROS (Robot Operating System) and LiDAR for SLAM capabilities.
* **Mobile App:** Converting the HTML dashboard into a native Android application.

## Coding Standards
* **C++ (Arduino):** Please comment your code thoroughly. Use descriptive variable names (e.g., `gasSensorAnalogPin` instead of `p1`). Avoid `delay()` at all costs; use `millis()` for timing.
* **Web:** Keep the dashboard lightweight. Do not add heavy frameworks (like React/Angular) unless absolutely necessary for a massive feature overhaul.

Let's build technology that saves lives!
