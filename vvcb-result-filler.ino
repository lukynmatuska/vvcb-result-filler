/*
 Display all the fast rendering fonts.

 This sketch uses the GLCD (font 1) and fonts 2, 4, 6, 7, 8
 
 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 3500

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

void setup(void)
{
  // Setup console
  Serial.begin(9600);
  Serial.println("Hello world!");

  tft.init();
  tft.setRotation(1);
  // First we test them with a background colour set
  tft.setTextSize(2);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Connecting to WiFi", 0, 0, 2);

  // Declare BUTTON_PIN as digital input
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  int i = 1;
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.println(".");
    tft.drawString(".", i++, 30, 2);
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Connected to WiFi", 0, 0, 2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(WiFi.localIP().toString().c_str(), 0, 30, 2);
  delay(WAIT);
}

void loop()
{
  tft.fillScreen(TFT_BLACK);

  double left;
  double right;
  do
  {
    left = millis() / 1000.0;
    delay(100);
    right = millis() / 1000.0;
    tft.fillScreen(TFT_BLACK);
    tft.drawString("L:", 0, 0, 4);
    tft.drawString(String(left), 45, 0, 4);
    tft.drawString("R:", 0, 40, 4);
    tft.drawString(String(right), 45, 40, 4);
    tft.drawString(WiFi.localIP().toString().c_str(), 0, 110, 2);
    delay(25);
  } while (digitalRead(BUTTON_PIN));

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED)
  {
    tft.drawString("WiFi OK", 0, 80, 2);
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"time\":{\"left\":" + String(left) + ",\"right\":" + String(right) + " }}");

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    tft.drawString("HTTP:" + String(httpResponseCode), 100, 80, 2);

    // Free resources
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
    tft.drawString("WiFi NOT OK", 0, 80, 2);
  }
  delay(WAIT);
}
