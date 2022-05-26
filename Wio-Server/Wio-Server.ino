#include <HTTPClient.h>
#include <rpcWiFi.h>
#include <WiFiClient.h>
#include "TFT_eSPI.h"
#include "SPI.h"
#include <cstdint>
#include <ExtFlashLoader.h>

const char* ssid = "HENSLICK";
const char* password = "K31960L11959";

const char* fanState = "http://192.168.1.168/state";
const char* fanOn = "http://192.168.1.168/on";
const char* fanOff = "http://192.168.1.168/off";
const char* smokeState = "http://192.168.1.229/state";
const char* smokeOn = "http://192.168.1.229/on";
const char* smokeOff = "http://192.168.1.229/off";

bool isOn;
int btnState = HIGH;
String Blowing;
String Smoking;

unsigned long previousMillis = 0;
const long interval = 5000;

TFT_eSPI tft = TFT_eSPI();

void coverText(int x, int y, int w, int h) {
  tft.drawRect(x, y, w, h, 0x0);
  tft.fillRect(x, y, w, h, 0x0);
}

void setup() {

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);
  // pinMode(WIO_5S_UP, INPUT_PULLUP);
  // pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  // pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  // pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  // pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  tft.begin();
  tft.setRotation(3);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);



  if ( digitalRead(WIO_KEY_A) == LOW) {
    ExtFlashLoader::ExtFlashLoader loader;
  }

  tft.drawString("Connecting", ((tft.width() / 3) - (tft.textWidth("Connecting") / 2)), (tft.height() / 2));
  delay(100);
   tft.drawString("Connecting.", ((tft.width() / 3) - (tft.textWidth("Connecting") / 2)), (tft.height() / 2));
    delay(100);
    tft.drawString("Connecting..", ((tft.width() / 3) - (tft.textWidth("Connecting") / 2)), (tft.height() / 2));
  delay(100);
    tft.drawString("Connecting...", ((tft.width() / 3) - (tft.textWidth("Connecting") / 2)), (tft.height() / 2));
      delay(100);
    tft.drawString("Connecting....", ((tft.width() / 3) - (tft.textWidth("Connecting") / 2)), (tft.height() / 2));
      delay(100);
    tft.drawString("Connecting.....", ((tft.width() / 3) - (tft.textWidth("Connecting") / 2)), (tft.height() / 2));
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
         delay(250);
  }
  tft.fillScreen(TFT_BLACK);
  isOn = true;
  Blowing = httpGETRequest(fanState);
  Smoking = httpGETRequest(smokeState);
  reDraw();
}

void loop() {

  unsigned long currentMillis = millis();

  checkPhysicalButton();

   if (currentMillis - previousMillis >= interval) {

    if ((WiFi.status() == WL_CONNECTED)) {
      isOn = true;
      // State = httpGETRequest(serverState);
      reDraw();
      previousMillis = currentMillis;
    }
    else {
      isOn = false;
      reDraw();
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

  if (httpResponseCode > 0) {
  
    payload = http.getString();
  }
  else {
    payload = "Disconnected";
  }
  
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
    coverText((tft.textWidth("WiFi: ") + 15) , (tft.height() / 4), tft.textWidth("Connected"), tft.fontHeight());
    tft.drawString("Connected", (tft.textWidth("WiFi: ") + 15), (tft.height() / 4));
  } else {
    coverText((tft.textWidth("WiFi: ") + 15), (tft.height() / 4), tft.textWidth("Disconnected"), tft.fontHeight());
    tft.drawString("Disconnected", (tft.textWidth("WiFi: ") + 15) , (tft.height() / 4));
  }

  tft.drawString("Smoke: ", 15, ((tft.height() / 4) + (tft.fontHeight() * 1.5)) );
  coverText((tft.textWidth("Smoke: ") + 15), ((tft.height() / 4) + (tft.fontHeight() * 1.5)), tft.textWidth("Disconnected"), tft.fontHeight());
  tft.drawString( Smoking, (tft.textWidth("Smoke: ") + 15) , ((tft.height() / 4) + (tft.fontHeight() * 1.5)));
  
    tft.drawString("Fan: ", 15, ((tft.height() / 4) + (tft.fontHeight() * 3)) );
  coverText((tft.textWidth("Fan: ") + 15), ((tft.height() / 4) + (tft.fontHeight() * 3)), tft.textWidth("Disconnected"), tft.fontHeight());
  tft.drawString( Blowing, (tft.textWidth("Fan: ") + 15) , ((tft.height() / 4) + (tft.fontHeight() * 3)));

}

void checkPhysicalButton()
{

  if (digitalRead(WIO_KEY_B) == LOW) {
    if (btnState != LOW) {
    if (Blowing.equals("Off")){
      Blowing = httpGETRequest(fanOn);
    } else {
      Blowing = httpGETRequest(fanOff);
    }
      reDraw();
    }
    btnState = LOW;
  } else if (digitalRead(WIO_KEY_C) == LOW) {

    if (btnState != LOW) {
        if (Smoking.equals("Off")){
      Smoking = httpGETRequest(smokeOn);
      } else {
      Smoking = httpGETRequest(smokeOff);
    }
      reDraw();
    }
    btnState = LOW;
  } else {
    btnState = HIGH;
  }
}
