#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// TESTING ONLY
bool shouldResetSettings = false;

bool shouldSaveConfig = false;
String ap_ssid;
String ap_pass;

void shouldSaveConfig();

void apConfiguration();
