#include "mqtt.h"

void handleIncommingMessage(char* topic, byte* payload, unsigned int lenght)
{
    Serial.print("MQTT message arrived: [");
    Serial.print(topic);
    Serial.print("] -- ");
    char _payload[lenght];
    for (int i = 0; i < lenght; i++)
    {
        _payload[i] = (char)payload[i];
    }
    Serial.print(_payload);

    // handle the messages
}

bool connect(int& device_uid)
{
    Serial.println("Attempting an MQTT connection.");

    if (client.connect(device_uid.c_str(), mqtt_user, mqtt_pass))
    {
        Serial.println("Connected.");
        subscribe()
        return true;
    }
    Serial.print("Failed, result code:");
    Serial.print(client.state());
    return false;
}

void subscribe()
{
    for (int i = 0; i < sizeof(topics); i++)
    {
        client.subscribe(topics[i].c_str())
        Serial.print("Subscribed to:");
        Serial.print(topics[i]);
    }
}