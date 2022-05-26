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
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);
  
  // Remove the password parameter, if you want the AP (Access Point) to be open
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
  digitalWrite(2,LOW);
    delay(500);
      digitalWrite(2,HIGH);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  MDNS.begin("fan");
  
   //if (!MDNS.begin("fan")) { //Start mDNS with name esp8266
   // Serial.println("Error setting up MDNS responder!");
  //  while (1) {
   //   delay(1000);
   // }
  //  }

     MDNS.addService("http", "tcp", 80);

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readState().c_str());
  //  Serial.println("Status Called");
  });
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
  digitalWrite(5,HIGH);
  request->send_P(200, "text/plain", readState().c_str());
  // Serial.println("on called");
  });
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(5,LOW);
    request->send_P(200, "text/plain", readState().c_str());
   // Serial.println("off called");
  });
  
  // Start server
  server.begin();
  
        digitalWrite(2,HIGH);
}
 
void loop(){
  
}
