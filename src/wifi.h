#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// TESTING ONLY
bool shouldResetSettings = false;

bool shouldSaveConfigFlag = false;
char ap_ssid[] = "TestSSID";
char ap_pass[] = "TestPASS";

void shouldSaveConfig();

void apConfiguration();

void printLocalIp();
