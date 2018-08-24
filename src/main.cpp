#include "config.cpp"
#include "device.cpp"
#include "mqtt.cpp"
#include "wifi.cpp"


void setup()
{
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT);

    genDeviceUid();

    if (readConfig())
    {
        Serial.println("Read config from chip storage.");
    }
    else
    {
        Serial.println("Error while reading config from chip storage.");
    }

    apConfiguration();

    if (shouldSaveConfigFlag)
    {
        if (writeConfig())
        {
            Serial.println("Wrote config on chip storage.");
        }
        else
        {
            Serial.println("Error while writing config to chip storage.");
        }
    }

    printLocalIp();

    mqtt_init();

    mqtt_connect();
}

void loop()
{
    if (!mqtt_isConnected())
    {
        int attemtps = 1;

        while (!mqtt_isConnected())
        {
            Serial.print("Attempting to connect to MQTT: ");
            Serial.print(attempts);

            mqtt_connect();
            delay(1500);

            attempts++;

            if (attempts == 5)
            {
                break;
            }
        }

        if (!mqtt_isConnected())
        {
            Serial.println("Resetting the chip.");
        }
    }

    mqtt_loop();
}
