#include <WiFi.h>
#include "time.h"
//#include "secret.h"

// data below is set as a define in the file secret.h
const char* ssid = "KJNMwifi3";
const char* password = "#TPLink3Nieuwww";
void WiFi_connect(){
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
printf(".");
}
printf(" CONNECTED\r\n");
}
void setup(){
WiFi_connect();
}
void loop(){
}
