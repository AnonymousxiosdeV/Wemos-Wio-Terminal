#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include <ESPAsyncTCP.h>
#include <AsyncElegantOTA.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#define motorD 0
#define motorSpeed 5


const char* ssid = "HENSLICK";
const char* password = "K31960L11959";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WiFiUDP Udp;

unsigned long topMillis;
unsigned long currentMillis;
bool startRunning;
const unsigned long topTime = 100;
unsigned long topWait = 1000;
const IPAddress dstIp(192, 168, 1, 149);  // EditThis: The destination for OSC messages.
const unsigned int dstPort = 9000;  // EditThis: The destination port for OSC messages.
const unsigned int localPort = 8765; // EditThis: The local port listening for inbound OSC

unsigned long previousMillis = 0;
unsigned long interval = 30000;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
        digitalWrite(2, LOW);
            delay(1000);
    digitalWrite(2, HIGH);
  }
  Serial.println(WiFi.localIP());
  //The ESP8266 tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

String readState() {
  if (digitalRead(5) == HIGH) {
    return String("TopOn");
  } else {
    return String("TopOff");
  }
}

void sendState(int m) {

  OSCMessage msg("/Top");
  msg.add(m);
  Udp.beginPacket(dstIp, dstPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

void setup() {
  randomSeed(analogRead(A0));
  pinMode(motorD, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(2, OUTPUT);
  startRunning = false;


  // Remove the password parameter, if you want the AP (Access Point) to be open
  Serial.begin(115200);
  
  initWiFi();

  while (WiFi.status() != WL_CONNECTED)
  {
    // Serial.println(".");
  }

  /*
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

  server.on("/topon", HTTP_GET, [](AsyncWebServerRequest * request) {
    topMillis = millis();
    startRunning == true;
    sendState(1);
    request->send_P(200, "text/plain", readState().c_str());
    // Serial.println("on called");
  });

  server.on("/topoff", HTTP_GET, [](AsyncWebServerRequest * request) {
    topMillis = 0;
    currentMillis = 0;
    startRunning = false;
    sendState(0);
    request->send_P(200, "text/plain", readState().c_str());
    // Serial.println("off called");
  });


  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hi! This is a sample response.");
  });

  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();

  Udp.begin(localPort);

}

void loop(void) {

  if (startRunning == true) {
    currentMillis = millis();
    autoRun();
  }
  
    unsigned long wifiMillis = millis();
  if (wifiMillis - previousMillis >=interval){
    switch (WiFi.status()){
      case WL_NO_SSID_AVAIL:
        Serial.println("Configured SSID cannot be reached");
        break;
      case WL_CONNECTED:
        Serial.println("Connection successfully established");
        break;
      case WL_CONNECT_FAILED:
        Serial.println("Connection failed");
        break;
    }
    Serial.printf("Connection status: %d\n", WiFi.status());
    Serial.print("RRSI: ");
    Serial.println(WiFi.RSSI());
    previousMillis = wifiMillis;
  }

}

void autoRun() {

  if (currentMillis - topMillis >= topWait) {
    digitalWrite(motorD, HIGH);
    digitalWrite(motorSpeed, HIGH);
    sendState(1);
    topMillis = currentMillis;
  }

  if (currentMillis - topMillis >= topTime) {
    digitalWrite(motorD, LOW);
    digitalWrite(motorSpeed, HIGH);    
    topWait = random(150, 3000);
    topMillis = currentMillis;
    sendState(0);
  }
}
