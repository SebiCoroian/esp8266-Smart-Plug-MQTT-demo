#include <FS.h>
#include <ArduinoJson.h>

bool readConfig()
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
                configFile.readBytes(bug.get(), size);

                DynamicJsonBuffer jsonBuffer;
                JsonObject& json = jsonBuffer.parseObject(buffer.get());

                if (json.succes())
                {
                    Serial.println("\nParsed JSON.");

                    // strcpy(_email, json["email"]);
                    device_uid = json["device_uid"].asString();
                    strcpy(device_id, json["device_id"]);
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
}

bool writeConfig()
{
    Serial.println("WRITING THE CONFIG FILE.");

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    // write relay state
    json["device_id"] = device_id.c_str();
    json["device_uid"] = device_uid.c_str();
    json["email"] = _email.c_str();

    File configFile = SPIFFS.open("/config,json", "w");

    if (!configFile) {
        Serial.println("Failed to open config file for writing.");
        return false;
    }

    json.printTo(configFile);
    configFile.close();
}
