#include <SPI.h>
#include <LoRa.h>

// =========================
// LORA PINS
// =========================
#define LORA_SS     5
#define LORA_RST    14
#define LORA_DIO0   26

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("     LORA RX DEBUG TEST");
  Serial.println("================================");

  Serial.println("Checking Pins...");
  Serial.print("SS   : "); Serial.println(LORA_SS);
  Serial.print("RST  : "); Serial.println(LORA_RST);
  Serial.print("DIO0 : "); Serial.println(LORA_DIO0);

  // =========================
  // SET LORA PINS
  // =========================
  LoRa.setPins(
    LORA_SS,
    LORA_RST,
    LORA_DIO0
  );

  Serial.println("Starting LoRa...");

  // =========================
  // START LORA
  // =========================
  if (!LoRa.begin(433E6)) {

    Serial.println("ERROR: LORA INIT FAILED!");

    Serial.println("Check:");
    Serial.println("- Power 3.3V");
    Serial.println("- Wiring");
    Serial.println("- NSS/CS");
    Serial.println("- Antenna");

    while (1);
  }

  Serial.println("SUCCESS: LoRa Started");
  Serial.println("RX READY");
  Serial.println("Waiting for packets...");
}

void loop() {

  int packetSize =
      LoRa.parsePacket();

  if (packetSize) {

    String msg = "";

    while (LoRa.available()) {
      msg += (char)LoRa.read();
    }

    int rssi =
        LoRa.packetRssi();

    float snr =
        LoRa.packetSnr();

    Serial.println();
    Serial.println("========== RX ==========");

    Serial.print("Packet Size : ");
    Serial.println(packetSize);

    Serial.print("Message     : ");
    Serial.println(msg);

    Serial.print("RSSI        : ");
    Serial.print(rssi);
    Serial.println(" dBm");

    Serial.print("SNR         : ");
    Serial.println(snr);

    Serial.println("STATUS      : RECEIVED");

    Serial.println("========================");
  }
}