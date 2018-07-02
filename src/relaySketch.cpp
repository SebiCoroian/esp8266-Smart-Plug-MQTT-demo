#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

//Variables to store MQTT server credentials and a random generated device id
char mqtt_server[40];
char mqtt_port[6];
char mqtt_user[30];
char mqtt_pass[30];
char group[30];
char name[30];
String device_uid = "device-" + String(random(0xffff), HEX);

// instance of MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

//flag for knowing if the new config should be saved or not
bool shouldSaveConfig = false;

// callback used by the wifi manager to announce that the config changed
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void handleIncommingMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char status[3];
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    status[i] = (char)payload[i];
  }
  Serial.println();

  if (!strcmp(topic, name)) {
    Serial.println("got individual message");
    setRelay(atoi(angle));
  } else if (!strcmp(topic, group)) {
    Serial.println("got message on group topic");
    setRelay(atoi(angle));
  }
}




//Relay functions def
void turnOnRelay();
void turnOffRelay();
void sendRelayState();
void setRelay();

const int relayPin = 5; // D1 pin

boolean relayState = false;

void setup()
{
	// initialize serial communication
	  Serial.begin(115200);
      Serial.println();
	  pinMode(relayPin, OUTPUT);


	  Serial.println("mounting FS...");
  // reads the contents of config.json if exists and populates the global variables mentioned above
  if (SPIFFS.begin()) { // open flash storage
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) { // check if config.json exists
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r"); // read contents of config.json
      if (configFile) { /// check if file not empty
        Serial.println("opened config file");
        // -- begin conversion of json to array --
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        // -- end conversion --
        if (json.success()) { // check if conversion was successful
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]); // populate mqtt_server from config
          strcpy(mqtt_port, json["mqtt_port"]); // populate mqtt_port from config
          strcpy(mqtt_user, json["mqtt_user"]); // populate mqtt_user from config
          strcpy(mqtt_pass, json["mqtt_pass"]); // populate mqtt_pass from config
          device_uid = json["device_uid"].asString(); // populate device id from config
          strcpy(group, json["group"]);
          strcpy(name, json["name"]);

        } else { // json conversion failed
          Serial.println("failed to load json config");
        }
      }
    }
  } else { // failed to mount flash storage
    Serial.println("failed to mount FS");
  }

  // -- begin configuration of the portal --

  // create custom parameters for the portal
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_port, 30);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt password", mqtt_pass, 30);
  WiFiManagerParameter custom_group("group", "gropup", group, 30);
  WiFiManagerParameter custom_name("name", "device name", name, 30);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set the callback used for notifying if there was a change
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // add custom parameters to portal§
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_name);
  wifiManager.addParameter(&custom_group);

  // reset portal settings. used for testing only. to force opening the portal on each boot.
  // wifiManager.resetSettings();

  // if the board could not connect to wifi on the first boot.
  // an access point called Configure Device would be created
  // when connecting to this access point a configuration portal would be displayed
  // where the user can set a new SSID and password for connecting to wifi
  // and the required credentials for mqtt.
  // this will run in a blocking loop so the boot process would stay here as long
  // as the board is not connected to wifi.
  if (!wifiManager.autoConnect("Configure Device")) {
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
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    strcpy(mqtt_user, custom_mqtt_user.getValue());
    strcpy(mqtt_pass, custom_mqtt_pass.getValue());
    strcpy(group, custom_group.getValue());
    strcpy(name, custom_name.getValue());

    // -- begin conversion of parameters to json and save them to flash storage --
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_user"] = mqtt_user;
    json["mqtt_pass"] = mqtt_pass;
    json["device_uid"] = device_uid.c_str();
    json["group"] = group;
    json["name"] = name;


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
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  // -- begin mqtt configuration --
  client.setServer(mqtt_server, atoi(mqtt_port)); // connect to host and port
  client.setCallback(handleIncommingMessage); // set handler for incomming messages on subscribed channels
  // -- end mqtt configuration --
}

// method used to reconnect the mqtt client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // connect to mqtt server with user and password and pass the device id,
    // device id should be unique on the bus so pay attention to this.
    if (client.connect(device_uid.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");

      // subscribe to what channels you want to listen to.
      client.subscribe(group);
      client.subscribe(name);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  // publish to the bus, a message used for auto discovery.
  Serial.println(("Device connected " + device_uid).c_str());
  client.publish("connected-device", name);
  if (group) {
    client.publish("connected-in-group", group);
  }

  
}
void loop()
{
 // as long as the mqtt client is not connected, reconnect.
  if (!client.connected()) {
      reconnect();
  }
  // on each iteration of loop, mqtt client would check the bus for new messages
  client.loop();
}



//function for relay's next status after message recived

void setRelay(int relayNext)
{
if(relayNext==0)
	turnOffRelay();
if(relayNext==1)
	turnOnRelay();
return;
}



//by aplying high voltage to the pin connected, the relay will relase AC

void turnOnRelay() 
{
 digitalWrite(relayPin, HIGH); // turn on relay with voltage HIGH 
 relayState = true;
        
  Serial.print("Relay On");
}

void turnOffRelay() {
  digitalWrite(relayPin, LOW);  // turn off relay with voltage LOW
  relayState = false;
     
  Serial.print("Relay off");
}
