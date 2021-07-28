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

unsigned long targetTime = 0; // Used for testing draw times

#define BUTTON_PIN 15

#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

void setup(void)
{
  // Setup console
  Serial.begin(9600);
  Serial.print("Ahoj svete!");

  tft.init();
  tft.setRotation(1);

  // Declare BUTTON_PIN as digital input
  pinMode(BUTTON_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  targetTime = millis();

  // First we test them with a background colour set
  tft.setTextSize(2);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  String LP = String(millis());
  String PP = String(millis());
  do
  {
    LP = String(millis());
    PP = String(millis());
    Serial.println(millis());
    tft.fillScreen(TFT_BLACK);
    tft.drawString("LP:", 0, 0, 4);
    tft.drawString(LP, 80, 0, 4);
    tft.drawString("PP:", 0, 40, 4);
    tft.drawString(PP, 100, 40, 4);
    delay(125);
  } while (!digitalRead(BUTTON_PIN));

  tft.fillScreen(TFT_BLACK);
  tft.drawString("posilam", 0, 0, 4);
  tft.drawString("na server", 0, 40, 4);

  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED)
  {
    tft.drawString("WiFi OK", 0, 80, 2);
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"time\":{\"left\":" + LP + ",\"right\":" + PP + " }}");

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    tft.drawString("HTTP:" + String(httpResponseCode), 0, 100, 2);

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
