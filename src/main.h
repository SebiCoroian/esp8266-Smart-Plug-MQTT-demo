#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define MAIN_H

namespace config
{
    bool readConfig();
    bool writeConfig();
}

namespace device
{
    #define LED 2
    #define ON 1
    #define OFF 0
    #define RELAY_PIN 5 // D1

    bool relayState = false;
    char id[30];
    String uid;
    char _email[30];

    void setRelayState(bool state);
    void genDeviceUid();
}

namespace mqtt
{
    // MQTT credentials
    char server[] = "m20.cloudmqtt.com";
    unsigned int port = 18805;
    char user[] = "dzdmwzut";
    char pass[] = "XzKi2HE7KyKt";

    // MQTT topics
    String topics[] = {"ID1", "GROUP1", "GROUP2", "GROUP3"};

    // MQTT client
    WiFiClient espClient;
    PubSubClient client(espClient);

    // Incoming message callback
    void handleIncommingMessage(char* topic, byte* payload, unsigned int lenght);
    bool connect();
    void subscribe();
    void init();
    void loop();
    bool isConnected();
    void publish(const char *topic, const char *payload);
}

namespace wifi
{
    // TESTING ONLY
    bool shouldResetSettings = false;

    bool shouldSaveConfigFlag = false;
    char ap_ssid[] = "TestSSID";
    char ap_pass[] = "TestPASS";

    void shouldSaveConfigCallback();
    void apConfiguration();
    void printLocalIp();
}
