#include "esp_camera.h"
#include <WiFi.h>
// #include <ArduinoWebsockets.h>
#include <WebSocketsClient.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include "driver/gpio.h"


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
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


char * url = "ws://192.168.0.104:9500/esp32cam";

camera_fb_t * fb = NULL;
size_t _jpg_buf_len = 0;
uint8_t * _jpg_buf = NULL;
uint8_t state = 0;

// using namespace websockets;
WebSocketsClient client;

///////////////////////////////////CALLBACK FUNCTIONS///////////////////////////////////
// void onMessageCallback(WebsocketsMessage message) {
//   Serial.print("Got Message: ");
//   Serial.println(message.data());
// }

///////////////////////////////////INITIALIZE FUNCTIONS///////////////////////////////////
esp_err_t init_camera() {
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
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_XGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return err;
  }
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_VGA);
  Serial.println("Cam Success init");
  return ESP_OK;
};


esp_err_t init_wifi() {
  WiFi.begin("hotspot", "987654321");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  // client.onMessage(onMessageCallback);
  client.begin("192.168.0.103",9500,"/esp32cam");
  client.setReconnectInterval(1000);
  client.enableHeartbeat(2000, 2000, 2);
  // bool connected =
  // if (!connected) {
  //   Serial.println("Cannot connect to websocket server!");
  //   state = 3;
  //   return ESP_FAIL;
  // }

  // if (state == 3) {
  //   return ESP_FAIL;
  // }

  return ESP_OK;
};


///////////////////////////////////SETUP///////////////////////////////////
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  //  disableCore0WDT();

  Serial.begin(115200);
  init_camera();
  init_wifi();
}

///////////////////////////////////MAIN LOOP///////////////////////////////////
void loop() {
  client.loop();
  if (client.isConnected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      esp_camera_fb_return(fb);
      // ESP.restart();
    }
    
    client.sendBIN((const uint8_t*) fb->buf, fb->len);
    Serial.println("MJPG sent");
    esp_camera_fb_return(fb);
    delay(100); 
  }
  
}