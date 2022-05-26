#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESP8266mDNS.h>


const char* ssid = "HENSLICK";
const char* password = "K31960L11959";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String readState() {
 if (digitalRead(5) == HIGH){
  return String("On");
 } else {
  return String("Off");
 }
}

void setup(){
  // Serial port for debugging purposes
  pinMode(5,OUTPUT);
  pinMode(2,OUTPUT);
  
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED)
  {
  digitalWrite(2,HIGH);
    delay(500);
      digitalWrite(2,LOW);
  }
  
  if (MDNS.begin("FAN")) {  //Start mDNS with name esp8266

    }

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readState().c_str());
  });
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
  digitalWrite(5,HIGH);
  request->send_P(200, "text/plain", readState().c_str());
  });
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(5,LOW);
    request->send_P(200, "text/plain", readState().c_str());
  });
  
  // Start server
  server.begin();
  
        digitalWrite(2,LOW);
}
 
void loop(){
  
}
