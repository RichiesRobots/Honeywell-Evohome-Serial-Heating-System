#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "TimeLib.h"


const char* WIFI_SSID = "WIFI NETWORK NAME"; 
const char* WIFI_PASSWORD = "WIFI PASSWORD";


void initializeWiFi(String initialMessage) {

 int wifiReconnectCount = 0;
  
 Serial.println(initialMessage);

  
 Serial.println(WIFI_SSID);

 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 Serial.println("");

 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
  if(wifiReconnectCount > 4500){
     ESP.restart();
  }
  
  delay(500);
  Serial.print(".");
  wifiReconnectCount++;
 }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

void checkReconnectWifi(){

  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED)) {
   
    WiFi.disconnect();
    initializeWiFi("Reconnecting To ");
  }
}
