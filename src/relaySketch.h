#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Relay functions
void turnOnRelay();
void turnOffRelay();
void sendRelayState();
void setRelayS();

void saveConfigCallback();
void handleIncommingMessage(char* topic, byte* payload, unsigned int length);
void reconnect();

// Variables to store MQTT server credentials and a random generated device id
char mqtt_server[40];
char mqtt_port[6];
char mqtt_user[30];
char mqtt_pass[30];
char group[30];
char name[30];
// Temporary device name until it gets a proper name from the server
String device_uid = "device-" + String(random(0xffff), HEX);

// Instance of MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Flag for knowing if the new config should be saved or not
bool shouldSaveConfig = false;

const int relayPin = 5; // D1 pin
bool relayState = false;
