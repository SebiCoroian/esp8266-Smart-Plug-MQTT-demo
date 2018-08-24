#include "device.h"

void setRelayState(int relayState)
{
    if (relayState == 0) {
      digitalWrite(RELAY_PIN, HIGH);
      relayState = true;
      digitalWrite(LED, LOW);
      Serial.print("Relay ON.");
    }
    if (relayState == 1) {
      digitalWrite(RELAY_PIN, LOW);
      relayState = false;
      digitalWrite(LED, HIGH);
      Serial.print("Relay OFF.");
    }
    return;
}

void genDeviceUid()
{
    device_uid = "UID-" + String(random(0xffff), HEX);
}
