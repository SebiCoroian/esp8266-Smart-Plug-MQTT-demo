#include "relaySketch.h"

char mqtt_server[]="m20.cloudmqtt.com";
unsigned int mqtt_port=18805;
char mqtt_user[]="dzdmwzut";
char mqtt_pass[]="XzKi2HE7KyKt";
char group[30];
char name[30];
char devkey[30];
char devID[30];

String device_uid = "device-" + String(random(0xffff), HEX);

// void subs(char topicList[])
// {
// if(topicList!="")
// {
// char aux[20];
// int i=0;
// while(topicList[i]!=" " || i==strlen(topicList))
// {
//   aux[i]=topicList[i];
// }
// client.subscribe("aux");
//
//
// subs(topicList+i);
//
// }
// }
