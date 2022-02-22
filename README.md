# esp8266-Smart-Plug-MQTT-demo
Just an Arduino code that controls a smart plug wireless, via esp8266 module using MQTT technology

# For this particular smart-plug I have followed the sketch below

![sketch](https://github.com/SebiCoroian/esp8266-Smart-Plug-MQTT-demo/blob/master/SmartPlug.png)
#
#
# The device will create an access point. Once you connect, you'll have to introduce the MQTT credentials. A unique ID is automatically created for each plug
# You can control one plug at a time or a group of plugs

## Here is a list of the components I have used:
 * NodeMCU dev board with esp8266 WI-FI module
   * https://goo.gl/LtWB7m
 * Solid State Relay
   * https://goo.gl/ov2d6e
 * AC-DC 110/220v to 3.3v transformer
   * https://goo.gl/7YU4Rb
 * Socket outlet (EU)
   * https://goo.gl/iSJ3aL
  ## Dependencies:
* [WiFiManager](https://github.com/tzapu/WiFiManager)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [PubSubClient](https://github.com/knolleary/pubsubclient)
