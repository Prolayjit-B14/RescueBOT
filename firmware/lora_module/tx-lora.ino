#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// =====================================================
// SENSOR PINS
// =====================================================
#define VIBRATION_PIN 34
#define MQ2_PIN 27

#define FLAME1_PIN 25
#define FLAME2_PIN 33

#define TRIG_PIN 2
#define ECHO_PIN 4

#define GREEN_LED 14
#define RED_LED 12
#define BUZZER 15

#define SDA_PIN 21
#define SCL_PIN 22

// GPS UART2
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

// =====================================================
// LORA PINS
// =====================================================
#define LORA_SS    5
#define LORA_RST   13
#define LORA_DIO0  32

// =====================================================
// OBJECTS
// =====================================================
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);
Adafruit_MPU6050 mpu;

// =====================================================
// THRESHOLD VALUES
// =====================================================
int vibrationThreshold = 1500;
int gasThreshold = 1800;
int distanceThreshold = 20;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("\n==========================");
  Serial.println("SYSTEM STARTING...");
  Serial.println("==========================");

  // =====================================
  // GPIO
  // =====================================
  pinMode(FLAME1_PIN, INPUT);
  pinMode(FLAME2_PIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  // =====================================
  // MPU6050
  // =====================================
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("Checking MPU6050...");

  if (!mpu.begin()) {
    Serial.println("ERROR: MPU6050 NOT FOUND!");
    while (1);
  }

  Serial.println("MPU6050 OK");

  // =====================================
  // GPS
  // =====================================
  gpsSerial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX_PIN,
    GPS_TX_PIN
  );

  Serial.println("GPS Started");

  // =====================================
  // LoRa
  // =====================================
  LoRa.setPins(
    LORA_SS,
    LORA_RST,
    LORA_DIO0
  );

  Serial.println("Starting LoRa...");

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa INIT FAILED!");
    while (1);
  }

  Serial.println("LoRa Started");
  Serial.println("SYSTEM READY");
}

void loop() {

  bool alert = false;

  // =====================================
  // SENSOR READINGS
  // =====================================
  int vibrationValue = analogRead(VIBRATION_PIN);
  int gasValue = analogRead(MQ2_PIN);

  int flame1 = digitalRead(FLAME1_PIN);
  int flame2 = digitalRead(FLAME2_PIN);

  // =====================================
  // ULTRASONIC
  // =====================================
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  float distance = 999;

  if (duration > 0) {
    distance = duration * 0.034 / 2.0;
  }

  // =====================================
  // MPU6050
  // =====================================
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // =====================================
  // GPS UPDATE
  // =====================================
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  float latitude = 0.0;
  float longitude = 0.0;

  if (gps.location.isValid()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
  }

  // =====================================
  // ALERT CONDITIONS
  // =====================================
  if (vibrationValue > vibrationThreshold)
    alert = true;

  if (gasValue > gasThreshold)
    alert = true;

  if (flame1 == LOW || flame2 == LOW)
    alert = true;

  if (distance > 0 && distance < distanceThreshold)
    alert = true;

  if (abs(ax) > 15 || abs(ay) > 15)
    alert = true;

  // =====================================
  // OUTPUT CONTROL
  // =====================================
  digitalWrite(RED_LED, alert);
  digitalWrite(BUZZER, alert);
  digitalWrite(GREEN_LED, !alert);

  // =====================================
  // CREATE DATA PACKET
  // =====================================
  String data =
    "VIB=" + String(vibrationValue) +
    ",GAS=" + String(gasValue) +
    ",FL1=" + String(flame1) +
    ",FL2=" + String(flame2) +
    ",DIST=" + String(distance, 2) +
    ",AX=" + String(ax, 2) +
    ",AY=" + String(ay, 2) +
    ",AZ=" + String(az, 2) +
    ",LAT=" + String(latitude, 6) +
    ",LON=" + String(longitude, 6) +
    ",ALERT=" + String(alert);

  // =====================================
  // SEND LORA PACKET
  // =====================================
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  // =====================================
  // SERIAL DEBUG
  // =====================================
  Serial.println("\n========== TX DATA ==========");
  Serial.println(data);
  Serial.println("=============================");

  delay(1000);
}