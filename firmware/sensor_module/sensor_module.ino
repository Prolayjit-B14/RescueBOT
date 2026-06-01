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
#define MQ2_PIN 35         // Analog

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

// -------- ALARM ON: Red LED on, buzzer tone (buzzer follows LED state) --------
void alertOn() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);

  // Buzzer is ONLY active when RED LED is HIGH — they are always in sync
  if (digitalRead(RED_LED) == HIGH) {
    tone(BUZZER, 2500); // 2.5kHz alarm tone
  }
}

// -------- ALARM OFF: Green LED on, buzzer silent --------
void alertOff() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);

  // Buzzer stops ONLY after RED LED is confirmed LOW
  if (digitalRead(RED_LED) == LOW) {
    noTone(BUZZER);
    digitalWrite(BUZZER, LOW);
  }
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
    client.setBufferSize(512); // Increase from default 256 to handle large JSON payloads
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
      doc["flag"]   = fireDetected ? 1 : 0;  // int flag for reliable frontend parsing
      doc["status"] = fireDetected ? "CRITICAL" : "OPTIMAL";
      doc["f1"]     = (flame1 == LOW) ? 1 : 0; // individual sensor states
      doc["f2"]     = (flame2 == LOW) ? 1 : 0;
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

      // 7. NEO-6M GPS Telemetry Packet (nested, on telemetry topic)
      StaticJsonDocument<384> docGps;
      char bufGps[384];
      docGps["sensor"] = "gps";
      JsonObject gpsVal = docGps.createNestedObject("value");
      bool hasFix = gps.location.isValid();
      gpsVal["lat"]     = hasFix ? gps.location.lat()    : 0.0;
      gpsVal["lng"]     = hasFix ? gps.location.lng()    : 0.0;
      gpsVal["speed"]   = hasFix ? gps.speed.kmph()      : 0.0;
      gpsVal["heading"] = hasFix ? gps.course.deg()      : 0.0;
      gpsVal["alt"]     = hasFix ? gps.altitude.meters() : 0.0;
      gpsVal["sats"]    = gps.satellites.isValid() ? gps.satellites.value() : 0;
      gpsVal["status"]  = hasFix ? ((gps.satellites.value() >= 4) ? "3D FIX" : "ACQUIRING") : "ACQUIRING";
      serializeJson(docGps, bufGps);
      client.publish(TOPIC_TELEMETRY, bufGps);

      // 7.1 Direct GPS Flat Broadcast Packet (flat JSON, on gps topic)
      StaticJsonDocument<256> gpsDoc;
      char gpsBuffer[256];
      gpsDoc["lat"]        = hasFix ? gps.location.lat()      : 0.0;
      gpsDoc["lng"]        = hasFix ? gps.location.lng()      : 0.0;
      gpsDoc["speed"]      = hasFix ? gps.speed.kmph()        : 0.0;
      gpsDoc["heading"]    = hasFix ? gps.course.deg()        : 0.0;
      gpsDoc["alt"]        = hasFix ? gps.altitude.meters()   : 0.0;
      gpsDoc["satellites"] = gps.satellites.isValid() ? gps.satellites.value() : 0;
      gpsDoc["sats"]       = gps.satellites.isValid() ? gps.satellites.value() : 0;
      gpsDoc["status"]     = hasFix ? ((gps.satellites.value() >= 4) ? "3D FIX" : "ACQUIRING") : "ACQUIRING";
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
    // Print GPS diagnostics to local Serial Monitor
    if (gps.charsProcessed() < 10) {
      Serial.println("GPS Module: OFFLINE / NO DATA RECEIVED! Check wiring (ESP32 RX2 Pin 16 connects to GPS TX, TX2 Pin 17 connects to GPS RX. Ensure TX/RX are not swapped!)");
    } else {
      int satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;
      if (gps.location.isValid()) {
        Serial.printf("GPS Module: ONLINE | 3D FIX | Lat: %0.6f | Lng: %0.6f | Satellites: %d\n", gps.location.lat(), gps.location.lng(), satellites);
      } else {
        Serial.printf("GPS Module: ONLINE | ACQUIRING... | Satellites: %d | (Needs outdoor view/clear sky for lock!)\n", satellites);
      }
    }

    // ---------------------------------------------------------------------------------------------
    // LOCAL PHYSICAL ALARM LOGIC — Buzzer fires ONLY when RED LED is ON
    // OR logic: alarm triggers if ANY single sensor is over its threshold
    // ---------------------------------------------------------------------------------------------
    bool alarmFire      = fireDetected;
    bool alarmGas       = (gas > 2000);
    bool alarmVibration = (vibrationIntensity > 50);
    bool alarmObstacle  = (distance < 20);

    bool isAlarmTriggered = alarmFire || alarmGas || alarmVibration || alarmObstacle;

    if (isAlarmTriggered) {
      alertOn(); // RED LED HIGH → buzzer follows LED state automatically
      Serial.println("[ALARM] Red Zone Threshold Breached!");
      if (alarmFire)      Serial.println("  -> Cause: FIRE DETECTED");
      if (alarmGas)       Serial.printf("  -> Cause: GAS LEVEL %d ppm\n", gas);
      if (alarmVibration) Serial.printf("  -> Cause: VIBRATION %d%%\n", vibrationIntensity);
      if (alarmObstacle)  Serial.printf("  -> Cause: OBSTACLE %ld cm\n", distance);

      if (WiFi.status() == WL_CONNECTED && client.connected()) {
        StaticJsonDocument<256> docAlert;
        char bufferAlert[256];
        docAlert["type"] = "ALERT";
        docAlert["conf"] = 99;
        // Report the highest-priority cause (fire > gas > vibration > obstacle)
        if (alarmFire) {
          docAlert["label"] = "FIRE";
          docAlert["desc"]  = "Active fire detected by flame sensors!";
        } else if (alarmGas) {
          docAlert["label"] = "GAS";
          docAlert["desc"]  = "Hazardous gas concentration detected!";
        } else if (alarmVibration) {
          docAlert["label"] = "VIBRATION";
          docAlert["desc"]  = "High vibration / seismic activity detected!";
        } else {
          docAlert["label"] = "OBSTACLE";
          docAlert["desc"]  = "Collision risk — obstacle within 20cm!";
        }
        serializeJson(docAlert, bufferAlert);
        client.publish(TOPIC_ALERTS, bufferAlert);
      }
    } else {
      alertOff(); // RED LED LOW → buzzer silenced automatically
    }
  }
}


