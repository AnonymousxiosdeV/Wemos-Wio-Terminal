#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"


// Set your access point network credentials
const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

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
  Serial.begin(115200);
  Serial.println();
  pinMode(5,OUTPUT);
  
  // Setting the ESP as an access point
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

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
}
 
void loop(){
  
}
