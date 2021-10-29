#define WAIT 3500

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "config.h"

double left = 0;
boolean leftButton = false;
double right = 0;
boolean rightButton = false;
int incomingByte = 0; // for incoming serial data

void setup(void)
{
  // Setup console
  Serial.begin(9600);
  Serial.println("Hello world!");

  // Declare buttons as digital input
  pinMode(LEFT_TARGET_PIN, INPUT_PULLUP);
  pinMode(RIGHT_TARGET_PIN, INPUT_PULLUP);

  // initialize the 16x2 lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("VVCB RSLTFLR");
  lcd.setCursor(0, 1);
  lcd.print("Hello world!");

  tft.init();
  tft.setRotation(1);
  // First we test them with a background colour set
  tft.setTextSize(2);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("VVCB RSLTFLR", 0, 0, 2);
  tft.drawString("Connecting to WiFi", 0, 20, 2);

  int i = 1;
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.println(".");
    tft.drawString(".", i++, 30, 2);
  }
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
  // send data only when you receive data:
  while (Serial.available() > 0)
  {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }

  // Prepare layout 16x2 LCD
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.setCursor(8, 1);
  lcd.print("R:");

  do
  {
    leftButton = !digitalRead(LEFT_TARGET_PIN);
    if (leftButton && left == 0)
    {
      left = millis() / 1000.0;
    }
    rightButton = !digitalRead(RIGHT_TARGET_PIN);
    if (rightButton && right == 0)
    {
      right = millis() / 1000.0;
    }
    // Time
    lcd.setCursor(0, 0);
    lcd.print(String(millis() / 1000.0));

    // Left target
    lcd.setCursor(2, 1);
    lcd.print(String(left));

    // Right target
    lcd.setCursor(10, 1);
    lcd.print(String(right));
  } while (left == 0 || right == 0);

  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED)
  {
    tft.drawString("WiFi OK", 0, 88, 2);
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"time\":{\"left\":" + String(left) + ",\"right\":" + String(right) + " }}");

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    tft.drawString("HTTP:" + String(httpResponseCode), 100, 88, 2);

    // Free resources
    http.end();
    left = 0;
    right = 0;
  }
  else
  {
    Serial.println("WiFi Disconnected");
    tft.drawString("WiFi NOT OK", 0, 80, 2);
  }
  delay(WAIT);
}
