#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"

/*
 * =================================================================================================
 * RescueBOT Robot: LIGHTWEIGHT CAMERA TEST FIRMWARE
 * =================================================================================================
 * Platform: ESP32-CAM (AI-Thinker)
 * 
 * Self-contained camera-only test firmware. Establishes WiFi & MQTT connections, 
 * hosts a local HTTP MJPEG server on port 81 (/stream), broadcasts the feed URL to 
 * the cloud MQTT broker to auto-mount, and responds to all Vision Tuning controls.
 * =================================================================================================
 */

// AI-Thinker Camera Pin Mapping
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define V_SYNC_GPIO_NUM   25
#define H_REF_GPIO_NUM    23
#define PCLK_GPIO_NUM     22

// Flash LED Pin
#define PIN_FLASH_LED      4   

// -------------------------------------------------------------------------------------------------
// [CONFIG] NETWORK & CLOUD
// -------------------------------------------------------------------------------------------------
// *** ENTER YOUR WIFI CREDENTIALS HERE ***
const char* ssid         = "YOUR_WIFI_SSID";
const char* password     = "YOUR_WIFI_PASSWORD";
const char* mqtt_broker  = "broker.emqx.io";

// Topic Map
const char* TOPIC_CAMERA    = "ares1/Robot/camera";
const char* TOPIC_COMMAND   = "ares1/Robot/command";
const char* TOPIC_STATUS    = "ares1/Robot/status";

// -------------------------------------------------------------------------------------------------
// [STATE & CONNECTIONS]
// -------------------------------------------------------------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);
httpd_handle_t stream_httpd = NULL;

bool isStreaming    = true; // Broadcasts live stream URL on boot
int ledIntensity    = 0;    // Flash LED brightness (0 - 255)
unsigned long lastUpdate = 0;
const int updateInterval = 2000; // Camera status broadcast interval

// -------------------------------------------------------------------------------------------------
// PWM LED FLASH DIMMING DRIVER (CORE v2 & v3 COMPATIBLE)
// -------------------------------------------------------------------------------------------------
void initLedFlash() {
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  ledcAttach(PIN_FLASH_LED, 5000, 8); // 5kHz frequency, 8-bit resolution (0-255)
  Serial.println("[LED] Core v3 LEDC PWM Driver Initialized.");
#else
  ledcSetup(0, 5000, 8);              // channel 0, 5kHz, 8-bit
  ledcAttachPin(PIN_FLASH_LED, 0);    // Bind GPIO 4 to Channel 0
  Serial.println("[LED] Core v2 LEDC PWM Driver Initialized.");
#endif
}

void setLedIntensity(int intensity) {
  ledIntensity = constrain(intensity, 0, 255);
#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
  ledcWrite(PIN_FLASH_LED, ledIntensity);
#else
  ledcWrite(0, ledIntensity);
#endif
  Serial.printf("[LED] Brightness: %d/255\n", ledIntensity);
}

// -------------------------------------------------------------------------------------------------
// LOCAL HTTP MJPEG STREAM SERVER (PORT 81)
// -------------------------------------------------------------------------------------------------
#define PART_BOUNDARY "123456789000000000000987654321"

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];

  res = httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=" PART_BOUNDARY);
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  Serial.println("[CAM] Web stream client connected.");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("[CAM] Capture failed");
      res = ESP_FAIL;
    } else {
      if (fb->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) {
          Serial.println("[CAM] JPEG conversion failed");
          res = ESP_FAIL;
        }
      } else {
        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;
      }
    }
    
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, "\r\n--" PART_BOUNDARY "\r\n", 36);
    }
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    
    if (res != ESP_OK) {
      Serial.println("[CAM] Stream connection closed.");
      break;
    }
  }
  return res;
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 81; 
  config.ctrl_port = 32769;

  httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
  };

  Serial.printf("[CAM] Starting HTTP server on port %u...\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    Serial.println("[CAM] Camera stream active.");
  }
}

// -------------------------------------------------------------------------------------------------
// CAMERA DRIVER INITIALIZATION
// -------------------------------------------------------------------------------------------------
void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = V_SYNC_GPIO_NUM;
  config.pin_href = H_REF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;          
    config.fb_count = 2;               
    config.grab_mode = CAMERA_GRAB_LATEST;
    Serial.println("[CAM] PSRAM Found. VGA Stream Enabled.");
  } else {
    config.frame_size = FRAMESIZE_QVGA; 
    config.jpeg_quality = 12;
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
    Serial.println("[CAM] No PSRAM. QVGA Low-Latency Stream Enabled.");
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[CAM] Camera driver failed: 0x%x\n", err);
    return;
  }
  
  sensor_t *s = esp_camera_sensor_get();
  if (s != NULL && s->id.PID == OV2640_PID) {
    s->set_vflip(s, 1);       
    s->set_hmirror(s, 1);     
  }
  Serial.println("[CAM] Camera hardware initialized.");
}

// -------------------------------------------------------------------------------------------------
// MQTT BROADCASTER (SENDS STREAM URL TO DASHBOARD)
// -------------------------------------------------------------------------------------------------
void publishCameraStatus() {
  StaticJsonDocument<256> doc;
  char buffer[256];
  
  doc["active"]  = isStreaming;
  doc["url"]     = "http://" + WiFi.localIP().toString() + ":81/stream";
  doc["fps"]     = isStreaming ? 30 : 0;
  doc["res"]     = psramFound() ? "VGA" : "QVGA";
  doc["quality"] = String(ledIntensity);
  
  serializeJson(doc, buffer);
  client.publish(TOPIC_CAMERA, buffer);
  Serial.printf("[MQTT] Published Stream URL: %s\n", buffer);
}

// -------------------------------------------------------------------------------------------------
// MQTT COMMAND RECEIVER (VISION TUNING CONTROLLER)
// ---------------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) return;
  
  const char* command = doc["command"];
  if (!command) return;

  Serial.printf("[MQTT] Command: %s\n", command);

  sensor_t *s = esp_camera_sensor_get();

  if (strcmp(command, "TOGGLE_STREAM") == 0) {
    isStreaming = doc["active"];
    publishCameraStatus();
  } 
  else if (strcmp(command, "SET_LED_INTENSITY") == 0) {
    int val = doc["val"];
    setLedIntensity(val);
    publishCameraStatus();
  }
  else if (strcmp(command, "SET_NIGHT_MODE") == 0) {
    bool enabled = doc["enabled"];
    setLedIntensity(enabled ? 255 : 0);
    publishCameraStatus();
  }
  else if (strcmp(command, "SET_RESOLUTION") == 0) {
    int val = doc["val"];
    if (s != NULL) {
      s->set_framesize(s, (framesize_t)val);
      publishCameraStatus();
    }
  }
  else if (strcmp(command, "SET_BRIGHTNESS") == 0) {
    int val = doc["val"];
    if (s != NULL) s->set_brightness(s, val);
  }
  else if (strcmp(command, "SET_CONTRAST") == 0) {
    int val = doc["val"];
    if (s != NULL) s->set_contrast(s, val);
  }
  else if (strcmp(command, "SET_SATURATION") == 0) {
    int val = doc["val"];
    if (s != NULL) s->set_saturation(s, val);
  }
  else if (strcmp(command, "SET_SPECIAL_EFFECT") == 0) {
    int val = doc["val"];
    if (s != NULL) s->set_special_effect(s, val);
  }
  else if (strcmp(command, "SET_HMIRROR") == 0) {
    bool enabled = doc["enabled"];
    if (s != NULL) s->set_hmirror(s, enabled ? 1 : 0);
  }
  else if (strcmp(command, "SET_VFLIP") == 0) {
    bool enabled = doc["enabled"];
    if (s != NULL) s->set_vflip(s, enabled ? 1 : 0);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("[MQTT] Connecting to broker...");
    String clientId = "RescueBOT_CamTest_" + WiFi.macAddress();
    
    if (client.connect(clientId.c_str())) {
      Serial.println("\n[MQTT] Connected!");
      client.subscribe(TOPIC_COMMAND);
      client.publish(TOPIC_STATUS, "{\"status\":\"ONLINE\",\"node\":\"camera_test_only\"}");
      publishCameraStatus();
    } else {
      delay(5000);
    }
  }
}

// -------------------------------------------------------------------------------------------------
// MAIN INITIALIZATION & RUNTIME LOOP
// ---------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("\n[RescueBOT] INITIALIZING LIGHTWEIGHT CAMERA TEST FIRMWARE...");

  initLedFlash();
  initCamera();

  WiFi.begin(ssid, password);
  WiFi.setSleep(false); // Disable sleep mode to preserve ultra low-latency streaming
  
  Serial.print("[NET] Connecting to WiFi AP");
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  Serial.println("\n[NET] Connected!");
  Serial.print("[NET] Stream IP: ");
  Serial.println(WiFi.localIP());

  startCameraServer();

  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
  
  Serial.printf("[NET] Stream Active at: 'http://%s:81/stream'\n", WiFi.localIP().toString().c_str());
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();
  if (now - lastUpdate > updateInterval) {
    lastUpdate = now;
    publishCameraStatus(); // Continuously update website sidebar stats & ensure IP is mounted
  }
}

