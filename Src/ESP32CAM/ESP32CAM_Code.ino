#include "esp_http_client.h"

#include "soc/soc.h"

#include "soc/rtc_cntl_reg.h"

#include "esp_camera.h"

#include <WiFi.h>

#include "Arduino.h"

#include "HTTPClient.h"

#include <PubSubClient.h>

#include <WiFiUdp.h>

#include <ArduinoJson.h>

#include <NTPClient.h>

//NTP Server setting to get the current date and time
const char * ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = 19800;

#define MQTT_HOST "<APPID>.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:<APPID>:<DEVICENAME>:<DEVICEID>"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "<TOKEN>"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"

#define api_key="<api key>" //ibm clodant storage API key to get IAM token
#define end_point="" //IBM object storage endpoint to store image

// MQTT objects
void callback(char * topic, byte * payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// variables to hold data
StaticJsonDocument < 200 > jsonDoc;
JsonObject payload = jsonDoc.to < JsonObject > ();
JsonObject status = payload.createNestedObject("d");
static char msg[200];

const char * ssid = ""; //<SSID
const char * password = "";//<Password>>

bool internet_connected = false;
long current_millis;
long last_capture_millis = 0;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM - 1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void callback(char * topic, byte * payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");

  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char * ) payload);
}

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  timeClient.begin();

  //camera configurations
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
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init( & config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Taking picture...");
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;

  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");

  }
  String apikey_s;
  //getting connection api key
  HTTPClient http;
  http.begin("https://iam.cloud.ibm.com/identity/token"); //Specify destination for HTTP request
  http.addHeader("content-type", " application/x-www-form-urlencoded");
  http.addHeader("accept", " application/json");

  int httpResponseCode = http.POST("response_type=cloud_iam&apikey=" +api_key +"&grant_type=urn:ibm:params:oauth:grant-type:apikey"); //Send the actual POST request
  delay(3000);
  if (httpResponseCode > 0) {
    const size_t capacity = 5000;
    DynamicJsonDocument doc(capacity);

    String response = http.getString(); //Get the response to the request
    Serial.println(httpResponseCode); //Print return code
    Serial.println(response); //Print request answer

    DeserializationError error = deserializeJson(doc, response);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    const char * apikey = doc["access_token"];
    Serial.println(apikey);
    apikey_s = String(apikey);
  } else {

    Serial.print("Error getting API key ");
    Serial.println(httpResponseCode);
    return;
  }
  ///image put
  timeClient.update();
  String fname = "AI_Farm_" + String(timeClient.getEpochTime()) + ".jpeg";
  String url_val = end_point + fname;
  HTTPClient http_a;
  http_a.begin(url_val); //Specify destination for HTTP request
  http_a.addHeader("Authorization", " Bearer " + String(apikey_s));
  http_a.addHeader("Content-Type", " application/octet-stream");
  Serial.println("PUT start");
  int httpResponseCode1 = http_a.PUT(fb -> buf, fb -> len);

  if (httpResponseCode1 > 0) {
    Serial.println("Success");
    String response = http_a.getString(); //Get the response to the request
    Serial.println(httpResponseCode1); //Print return code
    Serial.println(response);
  } else {
    Serial.println("PUT Fail");
    String response = http_a.getString(); //Get the response to the request
    Serial.println(httpResponseCode1); //Print return code
    Serial.println(httpResponseCode1);
  }
  http_a.end();

  // Connect to MQTT - IBM Watson IoT Platform
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);

  } else {
    Serial.println("MQTT Failed to connect!");
    //ESP.reset();
  }

  mqtt.loop();
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  Serial.println(url_val);
  status["image"] = url_val;
  serializeJson(jsonDoc, msg, 200);
  Serial.println(msg);
  if (!mqtt.publish(MQTT_TOPIC, msg)) {
    Serial.println("MQTT Publish failed");
  }

}

void loop() {
  // wait for WiFi connection

  delay(2000000);
  ESP.restart();
}