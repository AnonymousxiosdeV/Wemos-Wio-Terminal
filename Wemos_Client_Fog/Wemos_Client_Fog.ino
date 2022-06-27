#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESPAsyncTCP.h>
#include <AsyncElegantOTA.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

const char* ssid = "HENSLICK";
const char* password = "K31960L11959";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WiFiUDP Udp;

const IPAddress dstIp(192,168,1,149);     // EditThis: The destination for OSC messages.
const unsigned int dstPort = 9000;  // EditThis: The destination port for OSC messages.
const unsigned int localPort = 8764; // EditThis: The local port listening for inbound OSC

String readState() {
  if (digitalRead(5) == HIGH) {
    return String("FogOn");
  } else {
    return String("FogOff");
  }
}

void sendState(int m){
    
    OSCMessage msg("/Fog");
    msg.add(m);
    Udp.beginPacket(dstIp, dstPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

}

void setup() {
  
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);

  // Remove the password parameter, if you want the AP (Access Point) to be open
  //  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    // Serial.println(".");
    digitalWrite(2, LOW);
    delay(50);
    digitalWrite(2, HIGH);
  }

  /* Serial.println("");
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

    //  MDNS.addService("http", "tcp", 80);
  

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readState().c_str());
    //  Serial.println("Status Called");
  });
  
  */
  
  server.on("/fogon", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(5, HIGH);
    sendState(1);
    request->send_P(200, "text/plain", readState().c_str());
    // Serial.println("on called");
  });
  server.on("/fogoff", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(5, LOW);
        sendState(0);
    request->send_P(200, "text/plain", readState().c_str());
    // Serial.println("off called");
  });

/*
  server.on("/footstate", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readState2().c_str());
    //  Serial.println("Status Called");
  });
  
*/
 

  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();
  
  Udp.begin(localPort);

  digitalWrite(2, HIGH);
}

void loop(void) {

}
