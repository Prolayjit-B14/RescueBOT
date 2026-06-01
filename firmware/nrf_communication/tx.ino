#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// =====================================================
// NRF24
// =====================================================
RF24 radio(9, 10);   // CE, CSN
const byte address[6] = "00001";

// =====================================================
// JOYSTICK PINS
// =====================================================
#define ARM_X A0
#define ARM_Y A1

#define CAR_X A2
#define CAR_Y A3

#define GRIP_SWITCH 3

// =====================================================
// SETTINGS
// =====================================================
#define CAR_CENTER        512
#define CAR_DEADZONE      90

#define ARM_DEADZONE      70
#define DIAGONAL_ZONE     140

#define SMOOTHING         0.35

unsigned long lastPrint = 0;

// =====================================================
// ARM AUTO CENTER
// =====================================================
int armCenterX = 512;
int armCenterY = 512;

// =====================================================
// SMOOTHING
// =====================================================
float smoothCarX = 512;
float smoothCarY = 512;
float smoothArmX = 512;
float smoothArmY = 512;

// =====================================================
// DATA STRUCTURE
// MUST MATCH RX
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
  Serial.println("=================================");
  Serial.println("SMART NRF TRANSMITTER");
  Serial.println("CAR + ARM + GRIP CONTROL");
  Serial.println("=================================");

  pinMode(GRIP_SWITCH, INPUT_PULLUP);

  // NRF
  if (!radio.begin())
  {
    Serial.println("NRF24 NOT FOUND!");
    while (1);
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.stopListening();

  Serial.print("Packet Size: ");
  Serial.print(sizeof(DataPacket));
  Serial.println(" bytes");

  Serial.println("NRF READY");

  calibrateArmJoystick();

  Serial.println("SYSTEM READY");
}

// =====================================================
// LOOP
// =====================================================
void loop()
{
  readJoysticks();

  detectCarMovement();
  detectArmMovement();
  readGripSwitch();

  bool success = radio.write(&data, sizeof(data));

  if (millis() - lastPrint > 500)
  {
    lastPrint = millis();

    Serial.println();
    Serial.println("==============================");

    Serial.print("CAR X=");
    Serial.print(data.carX);

    Serial.print(" Y=");
    Serial.println(data.carY);

    Serial.print("CAR MOVE: ");
    Serial.println(CAR_LABELS[data.carMove]);

    Serial.println("------------------------------");

    Serial.print("CENTER X=");
    Serial.print(armCenterX);

    Serial.print(" Y=");
    Serial.println(armCenterY);

    Serial.print("ARM X=");
    Serial.print(data.armX);

    Serial.print(" Y=");
    Serial.println(data.armY);

    Serial.print("ARM MOVE: ");
    Serial.println(ARM_LABELS[data.armMove]);

    Serial.println("------------------------------");

    Serial.print("GRIP: ");
    Serial.println(data.grip ? "CLOSE" : "OPEN");

    Serial.print("TX STATUS: ");
    Serial.println(success ? "OK" : "FAILED");

    Serial.println("==============================");
  }

  delay(10);
}

// =====================================================
// CALIBRATE ARM JOYSTICK
// =====================================================
void calibrateArmJoystick()
{
  long sumX = 0;
  long sumY = 0;

  Serial.println();
  Serial.println("KEEP ARM JOYSTICK CENTER");
  Serial.println("CALIBRATING...");

  delay(1000);

  for (int i = 0; i < 100; i++)
  {
    sumX += analogRead(ARM_X);
    sumY += analogRead(ARM_Y);
    delay(5);
  }

  armCenterX = sumX / 100;
  armCenterY = sumY / 100;

  smoothArmX = armCenterX;
  smoothArmY = armCenterY;

  Serial.print("CENTER X = ");
  Serial.println(armCenterX);

  Serial.print("CENTER Y = ");
  Serial.println(armCenterY);

  Serial.println("CALIBRATION DONE");
}

// =====================================================
// READ JOYSTICKS
// =====================================================
void readJoysticks()
{
  smoothCarX +=
    (analogRead(CAR_X) - smoothCarX) *
    SMOOTHING;

  smoothCarY +=
    (analogRead(CAR_Y) - smoothCarY) *
    SMOOTHING;

  smoothArmX +=
    (analogRead(ARM_X) - smoothArmX) *
    SMOOTHING;

  smoothArmY +=
    (analogRead(ARM_Y) - smoothArmY) *
    SMOOTHING;

  data.carX = (int)smoothCarX;
  data.carY = (int)smoothCarY;

  data.armX = (int)smoothArmX;
  data.armY = (int)smoothArmY;
}

// =====================================================
// GRIP SWITCH
// =====================================================
void readGripSwitch()
{
  data.grip =
    (digitalRead(GRIP_SWITCH) == LOW)
    ? 1
    : 0;
}

// =====================================================
// CAR CONTROL
// =====================================================
void detectCarMovement()
{
  int x = data.carX - CAR_CENTER;
  int y = data.carY - CAR_CENTER;

  bool forward  = y > CAR_DEADZONE;
  bool backward = y < -CAR_DEADZONE;
  bool left     = x < -CAR_DEADZONE;
  bool right    = x > CAR_DEADZONE;

  if (forward && left &&
      abs(x) > DIAGONAL_ZONE)
    data.carMove = 5;

  else if (forward && right &&
           abs(x) > DIAGONAL_ZONE)
    data.carMove = 6;

  else if (backward && left &&
           abs(x) > DIAGONAL_ZONE)
    data.carMove = 7;

  else if (backward && right &&
           abs(x) > DIAGONAL_ZONE)
    data.carMove = 8;

  else if (forward)
    data.carMove = 1;

  else if (backward)
    data.carMove = 2;

  else if (left)
    data.carMove = 3;

  else if (right)
    data.carMove = 4;

  else
    data.carMove = 0;
}

// =====================================================
// ARM CONTROL (FIXED)
// =====================================================
void detectArmMovement()
{
  int xOffset =
    data.armX - armCenterX;

  int yOffset =
    data.armY - armCenterY;

  // DEADZONE FILTER
  if (abs(xOffset) < ARM_DEADZONE)
    xOffset = 0;

  if (abs(yOffset) < ARM_DEADZONE)
    yOffset = 0;

  // HOLD
  if (xOffset == 0 &&
      yOffset == 0)
  {
    data.armMove = 0;
    return;
  }

  // DOMINANT AXIS
  if (abs(xOffset) >
      abs(yOffset))
  {
    if (xOffset > 0)
      data.armMove = 2;
    else
      data.armMove = 1;
  }
  else
  {
    // fixed elbow direction
    if (yOffset > 0)
      data.armMove = 4;
    else
      data.armMove = 3;
  }
}