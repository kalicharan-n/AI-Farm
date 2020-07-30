# AI Farm using IBM Cloud- Call for code 2020

![Header Image](/Images/header.png)


## Contents

* Problem Statement
* Proposed Solution
* Architecture
* Hardware Requirements
* Software/Access Requirements
* Circuit
* Development/Code setup
* Release History
* Planned for future

## Problem Statement
Due to the change in unpridictable seasons, a farmer cant get better result by using his farm maintenance and irrigation techniques that he learned. Also, one of the key reasons for the shrinking ground water level is the cropping pattern, which is skewed toward crops that use more water. i.e. water requirement will vary based on crop selection and the evapotranspiration.  There are some solutions available, but, a farmer needs to invest lot of money in purchasing the hardware's and they need to replace their existing products with new one. Also those hardwards cant think, they will just do what you instruct you do to. 

Also, most of the manuals, operating procedure and farming knowledge base is available only in English, which most farmers in the world cant understand.

## Proposed Solution 

Developing IOT Based Intelligent Farm- For Better Irrigation, Crop Yield and Disease Control. Below products are used to solve the above mentioend problems,

* IBM watson visual recogniztion service- to predict the plant dieseses
* IBM watson Auto AI- Machine Learning Model- To predict the fertilizer needed
* IBM IOT- To control the irrigation pattern based on farm live weather and forecaseted weather informations
* IBM watson language translation- To translate the displayed page into farmer's local language
* Node Red on IBM- To develop the UI for a farmer

## Architecture

![Architecture Image](/Images/arch.PNG)

## Hardware Requirements
 * ESP32 CAM
 * ESP8266
 * Soil Mositure Sensor 
 * DHT11/22 temprature and humidity sensor
 * Rain sensor
 * Water pump
 * Water tube
 * Solar power supply/battery
 * PVC Pipe or wood for mounting the sensors and microcontrollers
 
 ![Product Image](/Images/Products.png)
 
 ## Software/Access Requirements
 
 * [IBM developer account](https://cloud.ibm.com/login)
 * IBM Visual recogniztion service 
 * IBM Watson Services(Auto AI, machine learning)
 * Node Red
 * [Twlio](https://www.twilio.com/) API access- for SMS services 
 * [IRRISAT](https://irrisat-cloud.appspot.com/api) API acccess- To get evapotranspiration rate for te given lattitude and longitude
 * [OpenWeather](https://openweathermap.org/api) API access- To retrieve weather informations (current weather, probability of precipitation and forecasting)
 * Arduino IDE- for IOT device programming
 
## Circuit Setup

### ESP8266 Setup

Here we are using two analog sensors(Soil moisture and Rain sensors). But, ESP8266 module has only one analog input (A0). So I Have connected the voltage pin of both sensors into data pins(D5,D6) to power up the sensors in run time. This will also reduce the total power consumption by the circuit.

![ESP8266 Circuit](/Images/ESP8266Circuit_Sensor.PNG)

### ESP32CAM Setup:

Connect IO0 and GND pins while uploading the code, and remove the connection after uploading the code.

![ESP32CAM Circuit](https://www.elementzonline.com/image/catalog/Blog_images/esp32-CAM/esp32_ttl.png)

### Actual Setup Images

![setup](/Images/RawSetupImages/setup.png)

![soil](/Images/RawSetupImages/soil.png)
![dht](/Images/RawSetupImages/dht.png)

## Development/Code setup

### Upload the sketch to ESP8266
Upload the source file in the /Src/ESP8266/ folder to your ESP8266 device using Arduino IDE
Make sure to update the below lines with your details
```
#define MQTT_HOST "<APPID>.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:<APPID>:<DEVICENAME>:<DEVICEID>"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "<TOKEN>"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"
 
char ssid[] = "<>"; // your network SSID (name)
char pass[] = "<>"; // your network password
```

### Upload the sketch to ESP32CAM
Upload the source file in the /Src/ESP32CAM/ folder to your ESP32CAM device using Arduino IDE
Ind addtion to above MQTT informations, you need to update the below IBM clodant storage informations
```
#define api_key="<api key>" //ibm clodant storage API key to get IAM token
#define end_point="" //IBM object storage endpoint to store image
```
### Setting Up Node Red

Import all the node red flows in the folder 'NodeRedFlows' into your NodeRed. Once imported, make sure to install/ update the Twilio, watson and openweather API keys.

If you imported everything correctly, you should see the below pages in your node red application.(Best viewd in mobile phone)

#### Home Page
![home](/Images/UISample/homepage.png)
#### Smart Irrigation Control Page
![irrigation](/Images/UISample/irrigation.png)
#### Historical data Page
![history](/Images/UISample/histrical.png)
#### Settings Page
You can update your basic informations in the below screen

![Settings](/Images/UISample/settings.png)

 ## Planned Update for future
  * Current solution works with only one plot/area. Next upgrade will handle more number of sensors and devices to handle big farms.
  * Improve the water savings by analyzing future rain forecasts
  * Add the ability to IOT devices to decide the farm/irrigation control by communicating each other in the same network, to address any critical problem(like our spine)
  * Predicting the crop yield based on historical data set and help the farmer to pick next crop based on yield and crop rotation
  * Flood and Hurricane management/alert system
  * Current solution only work for 'Corn'. This will be further extended to all other crops in incremental manner
  * OTA(Over The Air) code updates from IBM cloud to IOT devices for any patches- Currently, for any software update user need to connect to the devices manually using laptop. So it will take lot of time in case if there are more IOT nodes availabe in the same farm
