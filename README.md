# Ai Farm using IBM products- Call for code 2020

![Header Image](/Images/header.png)


## Contents

* Problem Statement
* Proposed Solution
* Architecture
* Hardware Requirements
* Software/Access Requirements
* Circuit
* Development setup
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
 
## Circuit Setup

ESP8266 Setup

Here we are using two analog sensors(Soil moisture and Rain sensors). But, ESP8266 module has only one analog input (A0). So I Have connected the voltage pin of both sensors into data pins(D5,D6) to power up the sensors in run time. This will also reduce the total power consumption by the circuit.

![ESP8266 Circuit](ESP8266Circuit_Sensor.PNG)

ESP32CAM Setup:

Connect IO0 and GND pins while uploading the code, and remove the connection after uploading the code.

![ESP32CAM Circuit](https://www.elementzonline.com/image/catalog/Blog_images/esp32-CAM/esp32_ttl.png)
