#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

/*
 * =================================================================================================
 * RescueBOT Robot: SENSOR TEST & MQTT TRANSMITTER FIRMWARE
 * =================================================================================================
 * Platform: ESP32 Dev Module / NodeMCU-32S
 * 
 * Integrates physical sensory telemetry (Gas, Vibration, Dual Flame, Ultrasonic, MPU6050, NEO-6M GPS)
 * with Wi-Fi network handshakes and MQTT broker publishers. Formats telemetry as JSON
 * payloads and feeds them to the EMQX broker to update the web dashboard in real-time.
 * =================================================================================================
 */

// -------- PIN DEFINITIONS --------
#define VIBRATION_PIN 34   // Analog
#define MQ2_PIN 32         // Analog

#define FLAME1_PIN 25
#define FLAME2_PIN 33

#define GREEN_LED 14
#define RED_LED 12

#define TRIG_PIN 2
#define ECHO_PIN 4

#define BUZZER 15

// -------------------------------------------------------------------------------------------------
// [CONFIG] NETWORK & CLOUD
// -------------------------------------------------------------------------------------------------
// *** ENTER YOUR WIFI CREDENTIALS HERE ***
const char* ssid         = "Redmi Note 11 Pro+ 5G";
const char* password     = "@polu1411P";
const char* mqtt_broker  = "broker.emqx.io";

// Topic Map
const char* TOPIC_TELEMETRY = "ares1/Robot/telemetry";
const char* TOPIC_STATUS    = "ares1/Robot/status";
const char* TOPIC_ALERTS    = "ares1/Robot/alerts";

// -------------------------------------------------------------------------------------------------
// [STATE & CONNECTIONS]
// -------------------------------------------------------------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastUpdate = 0;
const int updateInterval = 1000; // Telemetry transmit interval (1 second)

// GPS Hardware Serial
HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

// MPU6050
Adafruit_MPU6050 mpu;
bool mpuConnected = false;

// -------- ULTRASONIC --------
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout to avoid locking loop
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

// -------- ALERT (Red Alert, Passive/Active Buzzer Tone Support) --------
void alertOn() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  
  // Support both active and passive buzzers
  digitalWrite(BUZZER, HIGH); 
  tone(BUZZER, 2500); // Output a 2.5kHz frequency tone
}

// -------- NORMAL (Green Active, Buzzers Off) --------
void alertOff() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  
  digitalWrite(BUZZER, LOW);
  noTone(BUZZER); // Stop passive buzzer tone
}

// -------------------------------------------------------------------------------------------------
// MQTT BROKER CONNECTION MANAGER (Non-blocking retry limits)
// ---------------------------------------------------------------------------------
void reconnect() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  int attempts = 0;
  while (!client.connected() && attempts < 3) {
    Serial.print("[MQTT] Connecting to broker.emqx.io...");
    String clientId = "RescueBOT_Sensors_" + WiFi.macAddress();
    
    if (client.connect(clientId.c_str())) {
      Serial.println("\n[MQTT] Connected!");
      client.publish(TOPIC_STATUS, "{\"status\":\"ONLINE\",\"node\":\"sensor_payload_secondary\"}");
    } else {
      attempts++;
      Serial.println(" Failed. Retrying in 2 seconds...");
      delay(2000);
    }
  }
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);
  Serial.println("\n[RescueBOT] INITIALIZING SENSOR TELEMETRY FIRMWARE...");

  // Analog Sensory Modules (Vibration & MQ-2 are analog inputs)
  pinMode(VIBRATION_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);

  // Digital IR Flame Sensors (Flame is digital input)
  pinMode(FLAME1_PIN, INPUT);
  pinMode(FLAME2_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER, OUTPUT);

  alertOff(); // Default to safe indicator layout

  // GPS (RX Pin 16, TX Pin 17)
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("[GPS] NEO-6M Serial initialized.");

  // MPU6050 (SDA Pin 21, SCL Pin 22)
  Wire.begin(21, 22);
  bool mpuFound = mpu.begin(0x68, &Wire) || mpu.begin(0x69, &Wire);
  if (!mpuFound) {
    Serial.println("[MPU] MPU6050 accelerometer/gyro not found! Check wiring (I2C addr 0x68/0x69). Fallback enabled.");
    mpuConnected = false;
  } else {
    Serial.println("[MPU] MPU6050 sensor configured successfully.");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    mpuConnected = true;
  }

  // Non-blocking WiFi handshake with 15 seconds timeout
  WiFi.begin(ssid, password);
  Serial.print("[NET] Handshake with WiFi AP");
  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) { 
    delay(500); 
    Serial.print("."); 
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[NET] WiFi Connected successfully!");
    Serial.print("[NET] Assigned IP: ");
    Serial.println(WiFi.localIP());
    client.setServer(mqtt_broker, 1883);
  } else {
    Serial.println("\n[NET] WiFi Timeout! Running in local offline diagnostic alert mode.");
  }

  Serial.println("ESP32 Ready");
}

// -------- LOOP --------
void loop() {
  // Only handle network loops if WiFi is active
  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) reconnect();
    client.loop();
  }

  // 1. Continuously decode serial bytes from GPS to avoid NMEA overflow
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  unsigned long now = millis();
  if (now - lastUpdate > updateInterval) {
    lastUpdate = now;

    // Analog Sensors
    int vibration = analogRead(VIBRATION_PIN);
    
    // Read MQ2 Gas (raw analog 0-4095)
    int rawGas = analogRead(MQ2_PIN);
    
    // Auto-detect inverted logic (if it reads > 3000 in clean air, it's likely a DOUT pin or inverted AOUT)
    // We map it so that 0 is clean and higher values mean more gas.
    int gas = rawGas;
    if (rawGas > 3000) {
      gas = 4095 - rawGas; // Invert it so clean (4095) becomes 0, and gas (0) becomes 4095
    }

    // Digital Sensors (Active LOW)
    int flame1 = digitalRead(FLAME1_PIN);
    int flame2 = digitalRead(FLAME2_PIN);
    bool fireDetected = (flame1 == LOW || flame2 == LOW);

    long distance = getDistance();

    // MPU6050 Accelerometer & Gyro Events
    sensors_event_t a, g, temp;
    
    float pitch = 0.0;
    float roll = 0.0;
    float gyroMag = 0.0;
    float mpuTemp = 25.0; // Fallback normal room temp
    
    // Read MPU6050 safely if it responded in setup
    if (mpuConnected && mpu.getEvent(&a, &g, &temp)) {
      pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / M_PI;
      roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / M_PI;
      gyroMag = sqrt(g.gyro.x * g.gyro.x + g.gyro.y * g.gyro.y + g.gyro.z * g.gyro.z);
      mpuTemp = temp.temperature;
    }

    // Convert vibration to a more understandable 0-100% intensity scale
    int vibrationIntensity = map(vibration, 0, 4095, 0, 100);

    // ---------------------------------------------------------------------------------------------
    // BUILD AND PUBLISH JSON TELEMETRY TO MQTT (IF WIFI AND BROKER ARE CONNECTED)
    // ---------------------------------------------------------------------------------------------
    if (WiFi.status() == WL_CONNECTED && client.connected()) {
      StaticJsonDocument<256> doc;
      char buffer[256];

      // 1. GAS TELEMETRY
      doc.clear();
      doc["sensor"] = "gas";
      doc["value"]  = String(gas);
      doc["status"] = (gas > 2000) ? "CRITICAL" : "OPTIMAL";
      serializeJson(doc, buffer);
      client.publish(TOPIC_TELEMETRY, buffer);

      // 2. VIBRATION TELEMETRY
      doc.clear();
      doc["sensor"] = "vibration";
      doc["value"]  = String(vibrationIntensity);
      serializeJson(doc, buffer);
      client.publish(TOPIC_TELEMETRY, buffer);

      // 3. FLAME / FIRE TELEMETRY
      doc.clear();
      doc["sensor"] = "fire";
      doc["value"]  = fireDetected ? "FIRE DETECTED" : "CLEAR";
      doc["status"] = fireDetected ? "CRITICAL" : "OPTIMAL";
      serializeJson(doc, buffer);
      client.publish(TOPIC_TELEMETRY, buffer);

      // 4. ULTRASONIC RANGE DISTANCE
      doc.clear();
      doc["sensor"] = "ultrasonic";
      doc["value"]  = String(distance);
      serializeJson(doc, buffer);
      client.publish(TOPIC_TELEMETRY, buffer);

      // 5. MPU6050 TILT / ATTITUDE
      doc.clear();
      doc["sensor"] = "tilt";
      JsonObject tiltObj = doc.createNestedObject("value");
      tiltObj["pitch"] = String(pitch, 1);
      tiltObj["roll"]  = String(roll, 1);
      serializeJson(doc, buffer);
      client.publish(TOPIC_TELEMETRY, buffer);

      // 7. NEO-6M GPS Telemetry Packet
      doc.clear();
      doc["sensor"] = "gps";
      JsonObject gpsVal = doc.createNestedObject("value");
      if (gps.location.isValid()) {
        gpsVal["lat"]     = gps.location.lat();
        gpsVal["lng"]     = gps.location.lng();
        gpsVal["speed"]   = gps.speed.kmph();
        gpsVal["heading"] = gps.course.deg();
      } else {
        // Send 0.0 when no fix is available instead of hardcoding fake locations
        gpsVal["lat"]     = 0.0;
        gpsVal["lng"]     = 0.0;
        gpsVal["speed"]   = 0.0;
        gpsVal["heading"] = 0.0;
      }
      gpsVal["sats"]    = gps.satellites.isValid() ? gps.satellites.value() : 0;
      gpsVal["status"]  = (gps.satellites.value() >= 4) ? "3D FIX" : "ACQUIRING";
      serializeJson(doc, buffer);
      client.publish(TOPIC_TELEMETRY, buffer);

      // 7.1 Direct GPS Flat Broadcast Packet (Topic: ares1/Robot/gps)
      StaticJsonDocument<256> gpsDoc;
      char gpsBuffer[256];
      gpsDoc["lat"]        = gps.location.isValid() ? gps.location.lat() : 22.988684;
      gpsDoc["lng"]        = gps.location.isValid() ? gps.location.lng() : 88.453030;
      gpsDoc["speed"]      = gps.location.isValid() ? gps.speed.kmph() : 0.0;
      gpsDoc["heading"]    = gps.location.isValid() ? gps.course.deg() : 0.0;
      gpsDoc["alt"]        = gps.location.isValid() ? gps.altitude.meters() : 0.0;
      gpsDoc["satellites"] = gps.satellites.isValid() ? gps.satellites.value() : 0;
      gpsDoc["sats"]       = gps.satellites.isValid() ? gps.satellites.value() : 0;
      gpsDoc["status"]     = gps.location.isValid() ? ((gps.satellites.value() >= 4) ? "3D FIX" : "ACQUIRING") : "ACQUIRING";
      serializeJson(gpsDoc, gpsBuffer);
      client.publish("ares1/Robot/gps", gpsBuffer);
    }

    // Print values to local Serial Monitor for quick debug
    Serial.println("\n------ SENSOR TELEMETRY DIAGNOSTIC ------");
    Serial.printf("Vibration: ADC %d -> %d %%\n", vibration, vibrationIntensity);
    Serial.printf("MQ-2 Gas:  %d ppm\n", gas);
    Serial.printf("Flame1 (Raw): %s | Flame2 (Raw): %s\n", (flame1 == LOW) ? "FIRE" : "CLEAR", (flame2 == LOW) ? "FIRE" : "CLEAR");
    Serial.printf("Fire Status:  %s (OR logic: triggers if ANY flame sensor detects fire)\n", fireDetected ? "FIRE DETECTED!" : "CLEAR");
    Serial.printf("Distance:  %ld cm\n", distance);
    if (mpuConnected) {
      Serial.printf("MPU6050:   X: %0.1f deg | Y: %0.1f deg\n", pitch, roll);
      Serial.println("           -> (Note: X & Y will be 0.0 when flat on a table!)");
    } else {
      Serial.println("MPU6050:   OFFLINE / NOT DETECTED");
    }
    if (gps.location.isValid()) {
      Serial.printf("GPS Location: %0.6f, %0.6f | satellites: %d\n", gps.location.lat(), gps.location.lng(), gps.satellites.value());
    }

    // ---------------------------------------------------------------------------------------------
    // LOCAL PHYSICAL ALARM LOGIC & EMERGENCY ALERTS DISPATCH
    // ---------------------------------------------------------------------------------------------
    // ALARM TRIGGER: Use OR logic (||) to trigger if ANY of these conditions are true
    bool isAlarmTriggered = (vibrationIntensity > 50) || 
                            (gas > 2000) || 
                            fireDetected || 
                            (distance < 20);

    if (isAlarmTriggered) {
      alertOn();
      Serial.println("[ALARM ALERT] Red Zone Threshold Breached! (A sensor is over range)");

      if (WiFi.status() == WL_CONNECTED && client.connected()) {
        StaticJsonDocument<256> docAlert;
        char bufferAlert[256];
        docAlert.clear();
        docAlert["type"]  = "ALERT";
        docAlert["conf"]  = 99;
        if (fireDetected) {
          docAlert["label"] = "FIRE";
          docAlert["desc"]  = "Active fire detected by structural thermal sensors!";
        } else if (gas > 2000) {
          docAlert["label"] = "GAS";
          docAlert["desc"]  = "Hazardous atmospheric combustion gas concentration warning!";
        } else if (vibrationIntensity > 50) {
          docAlert["label"] = "VIBRATION";
          docAlert["desc"]  = "High vibration tremors detected inside the zone!";
        } else {
          docAlert["label"] = "OBSTACLE";
          docAlert["desc"]  = "Proximity collision alert — distance under critical threshold!";
        }
        serializeJson(docAlert, bufferAlert);
        client.publish(TOPIC_ALERTS, bufferAlert);
      }
    } else {
      alertOff();
    }
  }
}
