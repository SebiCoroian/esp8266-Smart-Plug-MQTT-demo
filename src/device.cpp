#include "device.h"

void setRelayState(int relayState)
{
    if (relayState == 0) {
      digitalWrite(relayPin, HIGH);
      relayState = true;
      digitalWrite(LED, LOW);
      Serial.print("Relay ON.");
    }
    if (relayState == 1) {
      digitalWrite(relayPin, LOW);
      relayState = false;
      digitalWrite(LED, HIGH);
      Serial.print("Relay OFF.");
    }
    return;
}

void genDeviceUid()
{
    device_uid = String(random(0xffff), HEX);
}
