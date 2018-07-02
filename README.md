# esp8266-Smart-Plug-MQTT-demo
Just an Arduino code that controls a smart plug wireless, via esp8266 module using MQTT technology

# For this particular smart-plug I have followed the sketch below, affiliated to this particular project https://medium.com/@monkeytypewritr/amazon-echo-esp8266-iot-a42076daafa5
![sketch](https://cdn-images-1.medium.com/max/1000/1*naLf9QOR8JYPTNeFEaywYg.png)
# 
# The only difference is that instead of Amazon Alexa, the wireless communication is realised via an MQTT server
#
# The device will create an acess point. Once you connect, you'll have to introduce the MQTT credentials. A unique ID is automatically created for each plug
# You can control one plug at a time or a group of plugs

## Here is a list of the components I have used:
* NodeMCU dev board with esp8266 module
  * https://goo.gl/LtWB7m
 * Solid State Relay
  * https://goo.gl/ov2d6e
 * AC-DC 110/220v to 3.3v transformer
  * https://goo.gl/7YU4Rb
 * Socket outlet (EU)
  * https://goo.gl/iSJ3aL
  ## Required libraries:
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [PubSubClient](https://github.com/knolleary/pubsubclient)


