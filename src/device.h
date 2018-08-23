
#define LED 2
#define ON 1
#define OFF 0

#define RELAY_PIN 5 // D1

bool relayState = false;
char device_id[30];
String device_uid;

void setRelayState(int relayState);
