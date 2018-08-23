
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// MQTT credentials
char mqtt_server[]="m20.cloudmqtt.com";
unsigned int mqtt_port=18805;
char mqtt_user[]="dzdmwzut";
char mqtt_pass[]="XzKi2HE7KyKt";

// MQTT topics
String topics[] = {"ID"};

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Incoming message callback
void handleIncommingMessage(char* topic, byte* payload, unsigned int lenght);

bool connect();

void subscribe();
