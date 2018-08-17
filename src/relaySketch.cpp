//#include "relaySketch.h"
#include "credentials.h"

// Function for relay's next status after message recived
void setRelay(int relayNext)
{
  if (relayNext == 0)
	   turnOffRelay();
  if (relayNext== 1)
	   turnOnRelay();
  return;
}

// By aplying high voltage to the pin connected, the relay will relase AC
void turnOnRelay()
{
  digitalWrite(relayPin, HIGH); // Turn on relay with voltage HIGH
  relayState = true;
  digitalWrite(LED, LOW);
  Serial.print("Relay On");
}

void turnOffRelay() {
  digitalWrite(relayPin, LOW);  // Turn off relay with voltage LOW
  relayState = false;
digitalWrite(LED, HIGH);
  Serial.print("Relay off");
}

// Callback used by the wifi manager to announce that the config changed
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void handleIncommingMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char status[30];
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    status[i] = (char)payload[i];
  }
  Serial.println();

  if (!strcmp(topic, "3")) {
    Serial.println("got individual message");

      // devID=status;
      // client.subscribe("devID");
        ///
                   ///
    setRelay(atoi(status)); ///
  } else if (strcmp(topic, "ID")) {
    
    strcpy(devID,status);
    client.subscribe("devID");
    Serial.println("New device ID");

  }
}

// Method used to reconnect the mqtt client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.println(name);

    Serial.println(mqtt_server);

    Serial.println(mqtt_port);

    Serial.println(mqtt_user);

    Serial.println(mqtt_pass);

    Serial.print("Attempting MQTT connection...");

    // Connect to mqtt server with user and password and pass the device ID,
    // Device ID should be unique on the bus so pay attention to this.
    if (client.connect(device_uid.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("connected");

      // Subscribe to what channels you want to listen to.
      //client.subscribe(group);
      client.subscribe("3");
      client.subscribe("ID");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  // Publish to the bus, a message used for auto discovery.
  Serial.println(("Device connected " + device_uid).c_str());
  client.publish("connected-device", name);
  client.publish("devkey: ", devkey);

}

void setup()
{
	// Initialize serial communication
	Serial.begin(115200);
  Serial.println();
	pinMode(relayPin, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);

	Serial.println("mounting FS...");
  // Reads the contents of config.json if exists and populates the global variables mentioned above
  if (SPIFFS.begin()) { // Open flash storage
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) { // Check if config.json exists
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r"); // Read contents of config.json
      if (configFile) { // Check if file not empty
        Serial.println("opened config file");
        // -- Begin conversion of json to array --
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        // -- End conversion --
        if (json.success()) { // Check if conversion was successful
          Serial.println("\nparsed json");

          // strcpy(mqtt_server, json["mqtt_server"]); // Populate mqtt_server from config
          // strcpy(mqtt_port, json["mqtt_port"]); // Populate mqtt_port from config
          // strcpy(mqtt_user, json["mqtt_user"]); // Populate mqtt_user from config
          // strcpy(mqtt_pass, json["mqtt_pass"]); // Populate mqtt_pass from config
          strcpy(devkey, json["devkey"]);
          device_uid = json["device_uid"].asString(); // Populate device id from config
          strcpy(group, json["group"]);
          strcpy(name, json["name"]);
          strcpy(devID, json["devID"]);


        } else { // Json conversion failed
          Serial.println("failed to load json config");
        }
      }
    }
  } else { // Failed to mount flash storage
    Serial.println("failed to mount FS");
  }

  // -- Begin configuration of the portal --

  // Create custom parameters for the portal
 // WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
 // WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
 // WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_port, 30);
 //  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt password", mqtt_pass, 30);
 // WiFiManagerParameter custom_group("group", "gropup", group, 30);
 //  WiFiManagerParameter custom_name("name", "device name", name, 30);
  WiFiManagerParameter custom_devkey("devkey", "user email", devkey, 30);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set the callback used for notifying if there was a change
  //wifiManager.setSaveConfigCallback(saveConfigCallback);

  // add custom parameters to portal§
 // wifiManager.addParameter(&custom_mqtt_server);
 // wifiManager.addParameter(&custom_mqtt_port);
 // wifiManager.addParameter(&custom_mqtt_user);
 // wifiManager.addParameter(&custom_mqtt_pass);
 //  wifiManager.addParameter(&custom_name);
 // wifiManager.addParameter(&custom_group);
  wifiManager.addParameter(&custom_devkey);

  // reset portal settings. used for testing only. to force opening the portal on each boot.
  wifiManager.resetSettings();

  // if the board could not connect to wifi on the first boot.
  // an access point called Configure Device would be created
  // when connecting to this access point a configuration portal would be displayed
  // where the user can set a new SSID and password for connecting to wifi
  // and the required credentials for mqtt.
  // this will run in a blocking loop so the boot process would stay here as long
  // as the board is not connected to wifi.
  if (!wifiManager.autoConnect("device-ap")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  // -- end configuration --

  //if you get here you have connected to the WiFi
  Serial.println("connected!");

  // if a new config was set in the portal, it will get saved as json on the flash storage.
  if (shouldSaveConfig) {

    // populate mqtt_server, mqtt_port, mqtt_user, mqtt_pass with the information provided by the user.
    // strcpy(mqtt_server, custom_mqtt_server.getValue());
    // strcpy(mqtt_port, custom_mqtt_port.getValue());
    // strcpy(mqtt_user, custom_mqtt_user.getValue());
    // strcpy(mqtt_pass, custom_mqtt_pass.getValue());
    // strcpy(group, custom_group.getValue());
    // strcpy(name, custom_name.getValue());

    // -- begin conversion of parameters to json and save them to flash storage --
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["devID"] = devID;
    json["device_uid"] = device_uid.c_str();
    json["group"] ="3";
    json["name"] = "3";
    json["devkey"] = devkey;


    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();

    // -- end conversion --
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  // in the scope of this demo we will control the built in led on the boards.
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  // -- begin mqtt configuration --
  client.setServer(mqtt_server, mqtt_port); // connect to host and port
  Serial.println(mqtt_server);
  Serial.println(mqtt_port);

  client.setCallback(handleIncommingMessage); // set handler for incomming messages on subscribed channels
  // -- end mqtt configuration --
}

void loop()
{
  //SPIFFS.format();
  // As long as the mqtt client is not connected, reconnect.
  if (!client.connected()) {

      reconnect();
  }
  // On each iteration of loop, mqtt client would check the bus for new messages
  client.loop();
}
