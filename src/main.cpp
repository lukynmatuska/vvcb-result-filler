#define WAIT 3500

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

#include <WiFi.h>
#include <WiFiClientSecure.h>
WiFiClientSecure client;
#include <HTTPClient.h>
#include "config.h"

double left = 0;
boolean leftButton = false;
double right = 0;
boolean rightButton = false;
int incomingByte = 0; // for incoming serial data

// Not sure if WiFiClientSecure checks the validity date of the certificate.
// Setting clock just to be sure...
void setClock()
{
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void drawConnectedToWifi(TFT_eSPI tft)
{
  tft.fillScreen(TFT_BLACK);
  tft.drawString("VVCB RSLTFLR", 0, 0, 2);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Connected to WiFi", 0, 30, 2);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(WiFi.localIP().toString().c_str(), 0, 60, 2);
}

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
  tft.drawString("Connecting to WiFi", 0, 30, 2);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  int i = 1;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.println(".");
    tft.drawString(".", i++, 60, 2);
  }

  // do not fail on SSL certificates
  client.setInsecure();
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  drawConnectedToWifi(tft);
  tft.drawString(String(WiFi.getHostname()).c_str(), 0, 90, 2);

  setClock();
  delay(WAIT);
}

void loop()
{
  /*// send data only when you receive data:
  while (Serial.available() > 0)
  {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }*/

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
    drawConnectedToWifi(tft);
    tft.drawString("WiFi OK", 0, 86, 2);

    HTTPClient https;
    // allow reuse (if server supports it)
    https.setReuse(true);

    Serial.println("[HTTPS] begin");
    if (https.begin(client, serverName))
    {
      // start connection and send HTTP header
      https.addHeader("Content-Type", "application/json");
      int httpResponseCode = https.POST("{\"time\":{\"left\":" + String(left) + ",\"right\":" + String(right) + " }}");

      // httpResponseCode will be negative on error
      if (httpResponseCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpResponseCode);
        tft.drawString("HTTPS:" + String(httpResponseCode), 100, 88, 2);

        // file found at server
        if (httpResponseCode == HTTP_CODE_OK || httpResponseCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          String payload = https.getString();
          Serial.println(payload);
        }
      }
      else
      {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpResponseCode).c_str());
      }

      https.end();
    }
    else
    {
      Serial.printf("[HTTPS] Unable to connect.\n");
    }
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
