# 🔧 Troubleshooting Guide

If you are building or testing the **RescueBOT** and run into issues, consult this guide for the most common problems and their solutions.

## Hardware Issues

### 1. ESP32 keeps restarting automatically (Brownout Detector)
* **Symptom:** The serial monitor repeatedly prints "Brownout detector was triggered".
* **Cause:** The ESP32 is not receiving a stable 3.3V or 5V. This usually happens when the motors or servos draw too much current.
* **Fix:** Do not power the servos directly from the ESP32 pins. Use an external 5V buck converter (LM2596) capable of delivering at least 2A to power the ESP32 and the servos independently of the motor driver.

### 2. NRF24L01 fails to connect or range is terrible
* **Symptom:** The receiver prints "Radio Not Available" or connection drops after 2 meters.
* **Cause:** The NRF24L01 is highly sensitive to power supply noise.
* **Fix:** Solder a 10uF to 100uF electrolytic capacitor directly across the VCC and GND pins on the NRF module itself. Ensure VCC is strictly 3.3V, not 5V.

### 3. ESP32-CAM video stream is stuttering or freezing
* **Cause:** Poor Wi-Fi signal, inadequate power supply, or thermal throttling.
* **Fix:** 
  1. Ensure the ESP32-CAM is powered with a stable 5V / 2A source.
  2. Lower the resolution in the code from UXGA to VGA or QVGA.
  3. Attach a small heatsink to the main chip on the ESP32-CAM.

## Software Issues

### 1. Dashboard MQTT status says "Disconnected"
* **Cause:** The JavaScript client cannot reach the MQTT broker.
* **Fix:**
  * If using a public broker (like HiveMQ), ensure your laptop/PC is connected to the internet.
  * Check the browser's developer console (F12) for WebSocket (WSS) errors. Sometimes corporate/university firewalls block port 8000. Try switching to a different broker (e.g., Mosquitto).

### 2. AI Human Detection isn't drawing bounding boxes
* **Cause:** The TensorFlow.js model hasn't loaded or isn't receiving the video feed properly.
* **Fix:** 
  * Open the browser console (F12). Look for errors related to loading the COCO-SSD model.
  * Ensure the `<img>` tag holding the ESP32-CAM stream has `crossorigin="anonymous"` set in the HTML, otherwise the canvas cannot read the image data due to CORS security policies.

### 3. ESP32-CAM won't accept code uploads
* **Symptom:** Arduino IDE says "Failed to connect to ESP32: Timed out waiting for packet header".
* **Fix:** 
  1. Ensure `GPIO 0` is connected to `GND` during upload.
  2. You MUST press the tiny physical RESET button on the back of the ESP32-CAM right when the IDE says "Connecting...".
