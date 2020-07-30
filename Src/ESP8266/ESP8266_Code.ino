#include <ESP8266WiFi.h>

#include <DHT.h>

#include <ArduinoJson.h>

#include <PubSubClient.h>
 // --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

#define MQTT_HOST "<APPID>.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:<APPID>:<DEVICENAME>:<DEVICEID>"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "<TOKEN>"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"

// Add GPIO pins used to connect devices
#define DHT_PIN 0 // GPIO pin the data line of the DHT sensor is connected to
#define SOIL_PIN 2

// Specify DHT11 (Blue) or DHT22 (White) sensor
#define DHTTYPE DHT11

// Add WiFi connection information
char ssid[] = "<>"; // your network SSID (name)
char pass[] = "<>"; // your network password

// --------------------------------------------------------------------------------------------
//        SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE
// --------------------------------------------------------------------------------------------
DHT dht(DHT_PIN, DHTTYPE);

// MQTT objects
void callback(char * topic, byte * payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonDocument < 100 > jsonDoc;
JsonObject payload = jsonDoc.to < JsonObject > ();
JsonObject status = payload.createNestedObject("d");
static char msg[70];

float h = 0.0; // humidity
float t = 0.0; // temperature

void callback(char * topic, byte * payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");

  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  const size_t capacity = 5000;
  DynamicJsonDocument doc(capacity);

  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  int mrun = doc["time"];
  Serial.println(mrun);
  //turn on the motor
  digitalWrite(D7, HIGH);
  for (i = 0; i <= mrun; i++) {
    delay(60000);
    digitalWrite(D5, HIGH);
    float percentage = 0.0;
    float reading = analogRead(A0);
    Serial.println(reading);
    percentage = (reading / 1024) * 100;
    percentage = map(reading, 550, 10, 0, 100);
    digitalWrite(D5, LOW);

    delay(1000);

    digitalWrite(D6, HIGH);
    int rain = analogRead(A0);
    Serial.println(rain);
    rain = map(rain, 0, 1024, 0, 100);
    digitalWrite(D6, LOW);

    h = dht.readHumidity();
    t = dht.readTemperature(); // uncomment this line for Celsius
    // t = dht.readTemperature(true); // uncomment this line for Fahrenheit

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // Print Message to console in JSON format
      int perc = i / mrun * 100;

      status["temp"] = t;
      status["humidity"] = h;
      status["soilMoist"] = percentage;
      status["rain"] = rain;

      if (rain > 80) {
        status["mstatus6"] = String(perc) + ",In Progress" + String(i) + "," + String(mrun - i);
      } else {
        status["mstatus6"] = String(perc) + ",Stopped due to rain" + String(i) + "," + String(mrun - i);
      }
      serializeJson(jsonDoc, msg, 70);
      Serial.println(msg);
      if (!mqtt.publish(MQTT_TOPIC, msg)) {
        Serial.println("MQTT Publish failed");
      }
      if (rain > 80) {
        //turn off if it is raining
        
        digitalWrite(D7, LOW);
  
        break;
      }
    }

  }
  digitalWrite(D7, LOW);
}
else {
  return;
}

}

void setup() {
  // Start serial console
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) {}
  Serial.println();
  Serial.println("ESP8266 Sensor Application");

  // Start WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");

  // Start connected devices
  dht.begin();

  // Connect to MQTT - IBM Watson IoT Platform
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);

  } else {
    Serial.println("MQTT Failed to connect!");
    ESP.reset();
  }
  pinMode(D5, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(A0, INPUT);
}

void loop() {
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
  digitalWrite(D5, HIGH);
  float percentage = 0.0;
  float reading = analogRead(A0);
  Serial.println(reading);
  percentage = (reading / 1024) * 100;
  percentage = map(reading, 550, 10, 0, 100);
  digitalWrite(D5, LOW);

  delay(1000);

  digitalWrite(D6, HIGH);
  int rain = analogRead(A0);
  Serial.println(rain);
  rain = map(rain, 0, 1024, 0, 100);
  digitalWrite(D6, LOW);

  h = dht.readHumidity();
  t = dht.readTemperature(); // uncomment this line for Celsius
  // t = dht.readTemperature(true); // uncomment this line for Fahrenheit

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Print Message to console in JSON format
    status["temp"] = t;
    status["humidity"] = h;
    status["soilMoist"] = percentage;
    status["rain"] = rain;
    serializeJson(jsonDoc, msg, 70);
    Serial.println(msg);
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
    }
  }
  // Pause - but keep polling MQTT for incoming messages
  for (int i = 0; i < 10; i++) {
    mqtt.loop();
    delay(1000);
  }
}