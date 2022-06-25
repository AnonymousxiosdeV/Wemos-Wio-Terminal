#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
// #include <ESP8266mDNS.h>
//#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <AsyncElegantOTA.h>

const char* ssid = "HENSLICK";
const char* password = "K31960L11959";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

unsigned long topMillis;
unsigned long footMillis;
unsigned long currentMillis;
bool startRunning;
const unsigned long footTime = 1000;
const unsigned long topTime = 150;
const unsigned long topWait = 1000;
const unsigned long footWait = 5000;

String readState() {
  if (digitalRead(5) == HIGH) {
    return String("On");
  } else {
    return String("Off");
  }
}

String readState2() {
  if (digitalRead(4) == HIGH) {
    return String("On");
  } else {
    return String("Off");
  }
}

String readAutoState() {
  if (startRunning == true) {
    return String("On");
  } else {
    return String("Off");
  }
}

void setup() {
  // Serial port for debugging purposes
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);

  startRunning = false;

  // Remove the password parameter, if you want the AP (Access Point) to be open
  //  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    digitalWrite(2, LOW);
    delay(500);
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
  */

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readState().c_str());
    //  Serial.println("Status Called");
  });
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(5, HIGH);
    request->send_P(200, "text/plain", readState().c_str());
    // Serial.println("on called");
  });
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(5, LOW);
    request->send_P(200, "text/plain", readState().c_str());
    // Serial.println("off called");
  });


  server.on("/state2", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readState2().c_str());
    //  Serial.println("Status Called");
  });
  server.on("/on2", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(4, HIGH);
    request->send_P(200, "text/plain", readState2().c_str());
    // Serial.println("on called");
  });
  server.on("/off2", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(4, LOW);
    request->send_P(200, "text/plain", readState2().c_str());
    // Serial.println("off called");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hi! This is a sample response.");
  });

  server.on("/autorun", HTTP_GET, [](AsyncWebServerRequest * request) {
    topMillis = millis();
    footMillis = millis();
    startRunning = true;
    request->send_P(200, "text/plain", readAutoState().c_str());
    // Serial.println("off called");
  });

  server.on("/autostop", HTTP_GET, [](AsyncWebServerRequest * request) {
    topMillis = 0;
    currentMillis = 0;
    footMillis = 0;
    startRunning = false;
    digitalWrite(5, LOW);
    digitalWrite(4, LOW);
    request->send_P(200, "text/plain", readAutoState().c_str());
    // Serial.println("off called");
  });

  AsyncElegantOTA.begin(&server);

  // Start server
  server.begin();

  digitalWrite(2, HIGH);
}

void loop(void) {

  if (startRunning == true) {
    currentMillis = millis();
    autoRun();
  }

}

void autoRun() {

  if ((digitalRead(5) == LOW) && (currentMillis - topMillis >= topWait)) {
    digitalWrite(5, HIGH);
    topMillis = currentMillis;
  }

  if ((digitalRead(5) == HIGH) && (currentMillis - topMillis >= topTime)) {
    digitalWrite(5, LOW);
    topMillis = currentMillis;
  }

  if ((digitalRead(4) == LOW) && (currentMillis - footMillis >= footWait)) {
    digitalWrite(4, HIGH);
    footMillis = currentMillis;
  }

  if ((digitalRead(4) == HIGH) && (currentMillis - footMillis >= footTime)) {
    digitalWrite(4, LOW);
    footMillis = currentMillis;
  }

}
