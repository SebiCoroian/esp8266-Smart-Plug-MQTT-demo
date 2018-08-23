#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

#define LED 2
#define ON 1
#define OFF 0

// Relay functions
void turnOnRelay();
void turnOffRelay();
void setRelay(int relayNext);

// wifi manager functions
void saveConfigCallback();

// mqtt functions
void handleIncommingMessage(char* topic, byte* payload, unsigned int length);
void reconnect();

char mqtt_server[]="m20.cloudmqtt.com";
unsigned int mqtt_port=18805;
char mqtt_user[]="dzdmwzut";
char mqtt_pass[]="XzKi2HE7KyKt";
char devkey[30];
char devID[30];

String device_uid = "device-" + String(random(0xffff), HEX);

// Instance of MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Flag for knowing if the new config should be saved or not
bool shouldSaveConfig = false;

const int relayPin = 5; // D1 pin
bool relayState = false;
