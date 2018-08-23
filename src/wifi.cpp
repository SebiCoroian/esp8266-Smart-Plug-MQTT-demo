#include "wifi.h"

void saveConfigCallback()
{
    Serial.print("Should save new config.")
    shouldSaveConfig = true;
}

void apConfiguration()
{
    WiFiManagerParameter token("Email", "User email", _email, 30);

    WiFiManager wifiManager;

    wifiManager.setSaveConfigCallback(saveConfigCallback());

    wifiManager.addParameter(&token);

    if (shouldResetSettings)
    {
        wifiManager.resetSettings();
    }

    if (wifiManager.autoConnect(ap_ssid, ap_pass)) {
        Serial.println("Failed to connect to wifi and hit timeout, resetting.");
        delay(3000);
        ESP.reset();
        delay(5000);
    }
}
