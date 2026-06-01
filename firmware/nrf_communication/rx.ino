#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// =====================================================
// NRF24
// =====================================================
RF24 radio(7, 8);   // CE, CSN
const byte address[6] = "00001";

// =====================================================
// MOTOR DRIVER
// =====================================================
#define IN1 2
#define IN2 3
#define IN3 9
#define IN4 10
#define ENA 5
#define ENB 6

// =====================================================
// SERVO PINS
// =====================================================
#define GRIP_PIN      A4
#define ELBOW_PIN     A5
#define SHOULDER_PIN  A2
#define SWEEP_PIN     A3

Servo gripServo;
Servo elbowServo;
Servo shoulderServo;
Servo sweepServo;

// =====================================================
// SERVO LIMITS
// =====================================================
#define GRIP_OPEN      0
#define GRIP_CLOSE     90

#define ELBOW_MIN      0
#define ELBOW_MAX      120

#define SHOULDER_MIN   0
#define SHOULDER_MAX   120

#define SWEEP_MIN      0
#define SWEEP_MAX      180

// =====================================================
// SETTINGS
// =====================================================
#define MOTOR_SPEED    120
#define TURN_SPEED     100
#define SERVO_STEP     5
#define TIMEOUT_MS     500

#define SWEEP_STEP     1
#define SWEEP_DELAY    15

// =====================================================
// VARIABLES
// =====================================================
int elbowAngle = 60;
int shoulderAngle = 60;

int sweepAngle = 0;
int sweepDir = 1;

unsigned long lastSweep = 0;
unsigned long lastReceive = 0;
unsigned long lastPrint = 0;

// =====================================================
// DATA STRUCTURE (MUST MATCH TX)
// =====================================================
struct DataPacket
{
  int carX;
  int carY;
  int armX;
  int armY;

  uint8_t carMove;
  uint8_t armMove;
  uint8_t grip;
};

DataPacket data;

// =====================================================
// LABELS
// =====================================================
const char* CAR_LABELS[] =
{
  "STOP",
  "FORWARD",
  "BACKWARD",
  "LEFT",
  "RIGHT",
  "FORWARD LEFT",
  "FORWARD RIGHT",
  "BACKWARD LEFT",
  "BACKWARD RIGHT"
};

const char* ARM_LABELS[] =
{
  "HOLD",
  "SHOULDER LEFT",
  "SHOULDER RIGHT",
  "ELBOW UP",
  "ELBOW DOWN"
};

// =====================================================
// SETUP
// =====================================================
void setup()
{
  Serial.begin(115200);

  Serial.println();
  Serial.println("================================");
  Serial.println("SMART NRF RECEIVER");
  Serial.println("CAR + ARM + GRIP");
  Serial.println("================================");

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopMotors();

  // Attach servos
  gripServo.attach(GRIP_PIN);
  elbowServo.attach(ELBOW_PIN);
  shoulderServo.attach(SHOULDER_PIN);
  sweepServo.attach(SWEEP_PIN);

  gripServo.write(GRIP_OPEN);
  elbowServo.write(elbowAngle);
  shoulderServo.write(shoulderAngle);
  sweepServo.write(sweepAngle);

  // NRF24
  if (!radio.begin())
  {
    Serial.println("NRF24 NOT FOUND!");
    while (1);
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.startListening();

  Serial.println("NRF READY");
  Serial.println("LISTENING...");
}

// =====================================================
// LOOP
// =====================================================
void loop()
{
  if (radio.available())
  {
    radio.read(&data, sizeof(data));

    lastReceive = millis();

    handleCarMove();
    handleArmMove();
    handleGrip();
  }

  // timeout safety
  if (millis() - lastReceive > TIMEOUT_MS)
  {
    stopMotors();
  }

  handleSweep();

  // Debug print
  if (millis() - lastPrint > 500)
  {
    lastPrint = millis();

    Serial.println();
    Serial.println("==============================");
    Serial.print("CAR: ");
    Serial.println(CAR_LABELS[data.carMove]);

    Serial.print("ARM: ");
    Serial.println(ARM_LABELS[data.armMove]);

    Serial.print("SHOULDER: ");
    Serial.println(shoulderAngle);

    Serial.print("ELBOW: ");
    Serial.println(elbowAngle);

    Serial.print("GRIP: ");
    Serial.println(data.grip ? "CLOSE" : "OPEN");

    Serial.print("SWEEP: ");
    Serial.println(sweepAngle);

    Serial.println("==============================");
  }

  delay(10);
}

// =====================================================
// ARM CONTROL (FIXED)
// =====================================================
void handleArmMove()
{
  switch (data.armMove)
  {
    case 1: // SHOULDER LEFT
      shoulderAngle -= SERVO_STEP;
      shoulderAngle = constrain(
        shoulderAngle,
        SHOULDER_MIN,
        SHOULDER_MAX
      );
      shoulderServo.write(shoulderAngle);
      break;

    case 2: // SHOULDER RIGHT
      shoulderAngle += SERVO_STEP;
      shoulderAngle = constrain(
        shoulderAngle,
        SHOULDER_MIN,
        SHOULDER_MAX
      );
      shoulderServo.write(shoulderAngle);
      break;

    case 3: // ELBOW UP
      elbowAngle += SERVO_STEP;
      elbowAngle = constrain(
        elbowAngle,
        ELBOW_MIN,
        ELBOW_MAX
      );
      elbowServo.write(elbowAngle);
      break;

    case 4: // ELBOW DOWN
      elbowAngle -= SERVO_STEP;
      elbowAngle = constrain(
        elbowAngle,
        ELBOW_MIN,
        ELBOW_MAX
      );
      elbowServo.write(elbowAngle);
      break;
  }
}

// =====================================================
// GRIP
// =====================================================
void handleGrip()
{
  if (data.grip == 1)
    gripServo.write(GRIP_CLOSE);
  else
    gripServo.write(GRIP_OPEN);
}

// =====================================================
// AUTO SWEEP
// =====================================================
void handleSweep()
{
  if (millis() - lastSweep < SWEEP_DELAY)
    return;

  lastSweep = millis();

  sweepAngle += sweepDir * SWEEP_STEP;

  if (sweepAngle >= SWEEP_MAX)
  {
    sweepAngle = SWEEP_MAX;
    sweepDir = -1;
  }

  if (sweepAngle <= SWEEP_MIN)
  {
    sweepAngle = SWEEP_MIN;
    sweepDir = 1;
  }

  sweepServo.write(sweepAngle);
}

// =====================================================
// CAR CONTROL
// =====================================================
void handleCarMove()
{
  switch (data.carMove)
  {
    case 1: moveForward(); break;
    case 2: moveBackward(); break;
    case 3: turnLeft(); break;
    case 4: turnRight(); break;
    case 5: forwardLeft(); break;
    case 6: forwardRight(); break;
    case 7: backwardLeft(); break;
    case 8: backwardRight(); break;
    default: stopMotors(); break;
  }
}

// =====================================================
// MOTOR FUNCTIONS
// =====================================================
void moveForward()
{
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void moveBackward()
{
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft()
{
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight()
{
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void forwardLeft()
{
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, MOTOR_SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void forwardRight()
{
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backwardLeft()
{
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, MOTOR_SPEED);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backwardRight()
{
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, TURN_SPEED);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}