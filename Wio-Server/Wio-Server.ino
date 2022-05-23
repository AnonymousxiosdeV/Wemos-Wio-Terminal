#include <HTTPClient.h>
#include <rpcWiFi.h>
#include <WiFiClient.h>
#include "TFT_eSPI.h"
#include "SPI.h"
#include <cstdint>
#include <ExtFlashLoader.h>

const char* ssid = "ESP8266-Access-Point";
const char* password = "123456789";

//Your IP address or domain name with URL path
const char* serverState = "http://192.168.4.1/state";
const char* serverOn = "http://192.168.4.1/on";
const char* serverOff = "http://192.168.4.1/off";

String State;
bool isOn;

unsigned long previousMillis = 0;
const long interval = 5000; 

void coverText(int x, int y, int w, int h) {
  tft.drawRect(x, y, w, h, 0x0);
  tft.fillRect(x, y, w, h, 0x0);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  
  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  


  if ( digitalRead(WIO_KEY_A) == LOW) {
    ExtFlashLoader::ExtFlashLoader loader;
  }
  
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  tft.drawString("Connecting..", ((tft.width() / 2) - (tft.textWidth("Connecting..") / 2)), (tft.height() / 2)); 
    delay(500);
    Serial.print(".");
  }
  tft.fillScreen(TFT_BLACK);
  isOn = true;
  Serial.println("");
  Serial.println("Connected to WiFi");
  reDraw();
}

void loop() {

  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
  
    if ((WiFi.status() == WL_CONNECTED)) {
      State = httpGETRequest(serverState);
      reDraw();
      
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
  // Serial.print("HTTP Response code: ");
  // Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void reDraw() {
  /******
    tft.drawString("Power: ", (tft.width() / 4) - (tft.textWidth("Power: ") / 2), (tft.height() / 4));
    if (isOn) {
     coverText((tft.width() / 4) + (tft.textWidth("Power: ") / 2), (tft.height() / 4), tft.textWidth("OFF"), tft.fontHeight());
     tft.drawString("ON", (tft.width() / 4) + (tft.textWidth("Power: ") / 2), (tft.height() / 4));
    } else {
     coverText((tft.width() / 4) + (tft.textWidth("Power: ") / 2), (tft.height() / 4), tft.textWidth("OFF"), tft.fontHeight());
     tft.drawString("OFF", (tft.width() / 4) + (tft.textWidth("Power: ") / 2), (tft.height() / 4));
    }
     tft.drawString("Brightness: ", (tft.width() / 4) - (tft.textWidth("Brightness: ") / 2), ((tft.height() / 4) + (tft.fontHeight() * 1.5)) );
    coverText((tft.width() / 4) + (tft.textWidth("Brightness: ") / 2), ((tft.height() / 4) + (tft.fontHeight() * 1.5)), tft.textWidth("5555"), tft.fontHeight());
    tft.drawNumber(brightness, (tft.width() / 4) + (tft.textWidth("Brightness: ") / 2), ((tft.height() / 4) + (tft.fontHeight() * 1.5)));
   ******/

  tft.drawString("WiFi: ", 15, (tft.height() / 4));
  if (isOn) {
    coverText((tft.textWidth("WiFi: ") + 15) , (tft.height() / 4), tft.textWidth("Is Connected"), tft.fontHeight());
    tft.drawString("Is Connected", (tft.textWidth("WiFi: ") + 15), (tft.height() / 4));
  } else {
    coverText((tft.textWidth("WiFi: ") + 15), (tft.height() / 4), tft.textWidth("Not Connected"), tft.fontHeight());
    tft.drawString("Not Connected", (tft.textWidth("WiFi: ") + 15) , (tft.height() / 4));
  }

  tft.drawString("Smoke: ", 15, ((tft.height() / 4) + (tft.fontHeight() * 1.5)) );
  coverText((tft.textWidth("Smoke: ") + 15), ((tft.height() / 4) + (tft.fontHeight() * 1.5)), tft.textWidth("5555"), tft.fontHeight());
tft.drawString( State, (tft.textWidth("WiFi: ") + 15) , (tft.height() / 4));

}

void checkPhysicalButton()
{

  if (digitalRead(WIO_KEY_B) == LOW) {
    // btnState is used to avoid sequential toggles
    if (btnState != LOW) {

      if (brightness >= 125) {
        brightness = 0;
        Blynk.virtualWrite(V0, (int)brightness);
      } else if (brightness <= 124) {
        brightness = 255;
        Blynk.virtualWrite(V0, (int)brightness);

      }
      Serial.print("Brightness is : ");
      Serial.println(brightness);

      reDraw();
    }
    btnState = LOW;
  } else if (digitalRead(WIO_KEY_C) == LOW) {
    // btnState is used to avoid sequential toggles
    if (btnState != LOW) {

      isOn = !isOn;
      Serial.print("Power is : ");
      Serial.println(isOn);

      // Update Button Widget
      Blynk.virtualWrite(V3, isOn);
      reDraw();
    }
    btnState = LOW;
  } else {
    btnState = HIGH;
  }
}
