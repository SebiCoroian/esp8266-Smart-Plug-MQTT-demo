#include "main.h"


bool config :: readConfig()
{
    Serial.println("READING THE CONFIG FILE.");

    Serial.println("Mounting file system.");

    if (SPIFFS.begin())
    {
        Serial.println("Mounted file system.");

        if (SPIFFS.exists("/config.json"))
        {
            Serial.println("Reading config file.");

            File configFile = SPIFFS.open("/config.json", "r");

            if (configFile)
            {
                Serial.println("Opened config file.");

                size_t size = configFile.size();
                std :: unique_ptr<char[]> buffer(new char[size]);
                configFile.readBytes(buffer.get(), size);

                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buffer.get());

                if (json.success())
                {
                    Serial.println("\nParsed JSON.");

                    // strcpy(_email, json["email"]);
                    device :: uid = json["device_uid"].asString();
                    strcpy(device :: id, json["device_id"]);
                    // parse relay state
                }
                else {
                    Serial.println("Failed to load JSON config");
                }
            }
        }
    }
    else
    {
        Serial.println("Failed to mount file system");
        return false;
    }
    return true;
}

bool config :: writeConfig()
{
    Serial.println("WRITING THE CONFIG FILE.");

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    // write relay state
    json["device_id"] = device :: id;
    json["device_uid"] = device :: uid.c_str();
    json["email"] = device :: _email;

    File configFile = SPIFFS.open("/config,json", "w");

    if (!configFile) {
        Serial.println("Failed to open config file for writing.");
        return false;
    }

    json.printTo(configFile);
    configFile.close();
    return true;
}



void device :: setRelayState(bool state)
{
    if (state) {
      digitalWrite(RELAY_PIN, HIGH);
      device :: relayState = true;
      digitalWrite(LED, LOW);
      Serial.print("Relay ON.");
    }
    else {
      digitalWrite(RELAY_PIN, LOW);
      device :: relayState = false;
      digitalWrite(LED, HIGH);
      Serial.print("Relay OFF.");
    }
    return;
}

void device :: genDeviceUid()
{
    device :: uid = "UID-" + String(random(0xffff), HEX);
}



void mqtt :: handleIncommingMessage(char* topic, byte* payload, unsigned int lenght)
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

    // Save the proper device ID that the chip
    // should listen to
    if (strcmp(topic, device :: uid.c_str()))
    {
        strcpy(device :: id, _payload);
        config :: writeConfig();
        mqtt :: topics[0] = strcat("ID", device :: id);
        config :: writeConfig();
        ESP.reset();
    }
}

void mqtt :: init()
{
    mqtt :: client.setServer(mqtt :: server, mqtt :: port);
    mqtt :: client.setCallback(mqtt :: handleIncommingMessage);
}

bool mqtt :: connect()
{
    Serial.println("Attempting an MQTT connection.");

    if (client.connect(device :: uid.c_str(), mqtt :: user, mqtt :: pass))
    {
        Serial.println("Connected.");
        mqtt :: subscribe();
        return true;
    }
    Serial.print("Failed, result code:");
    Serial.print(mqtt :: client.state());
    return false;
}

void mqtt :: subscribe()
{
    for (int i = 0; i < sizeof(topics); i++)
    {
        mqtt :: client.subscribe(mqtt :: topics[i].c_str());
        Serial.print("Subscribed to:");
        Serial.print(mqtt :: topics[i]);
    }
}

bool mqtt :: isConnected()
{
    return mqtt :: client.connected();
}

void mqtt :: loop()
{
    mqtt :: client.loop();
}

void mqtt :: publish(const char *topic, const char *payload)
{
    mqtt :: client.publish(topic, payload);
}



void wifi :: shouldSaveConfigCallback()
{
    Serial.print("Should save new config.");
    wifi :: shouldSaveConfigFlag = true;
}

void wifi :: apConfiguration()
{
    WiFiManagerParameter token("Email", "User email", device :: _email, 30);

    WiFiManager wifiManager;

    wifiManager.setSaveConfigCallback(wifi :: shouldSaveConfigCallback);

    wifiManager.addParameter(&token);

    if (wifi :: shouldResetSettings)
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

void wifi :: printLocalIp()
{
    Serial.print("Local IP: ");
    Serial.print(WiFi.localIP());
}


// --------------------
// -----BOARD----------
// --------------------

void setup()
{
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUILTIN_LED, OUTPUT);

    device :: genDeviceUid();

    if (config :: readConfig())
    {
        Serial.println("Read config from chip storage.");
    }
    else
    {
        Serial.println("Error while reading config from chip storage.");
    }

    wifi :: apConfiguration();

    if (wifi :: shouldSaveConfigFlag)
    {

        mqtt :: publish("GET_FINAL_ID", device :: _email);

        if (config :: writeConfig())
        {
            Serial.println("Wrote config on chip storage.");
        }
        else
        {
            Serial.println("Error while writing config to chip storage.");
        }
    }

    wifi :: printLocalIp();

    mqtt :: init();

    mqtt :: connect();
}

void loop()
{
    if (!mqtt :: isConnected())
    {
        // int attemtps = 1;

        while (!mqtt :: isConnected())
        {
            Serial.print("Attempting to connect to MQTT: ");
            // Serial.print(attempts);

            mqtt :: connect();
            delay(1500);

            // attempts++;
            //
            // if (attempts == 5)
            // {
            //     break;
            // }
        }

        if (!mqtt :: isConnected())
        {
            Serial.println("Resetting the chip.");
        }
    }

    mqtt :: loop();
}
