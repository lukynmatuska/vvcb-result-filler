/*
 Display all the fast rendering fonts.

 This sketch uses the GLCD (font 1) and fonts 2, 4, 6, 7, 8
 
 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

// New background colour
#define TFT_BROWN 0x38E0

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 1024

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

unsigned long targetTime = 0; // Used for testing draw times

void setup(void)
{
  tft.init();
  tft.setRotation(1);
}

void loop()
{
  targetTime = millis();

  // First we test them with a background colour set
  tft.setTextSize(2);

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);

  tft.drawString("LP:0:12,34", 0, 0, 4);
  tft.drawString("PP:1:56,78", 0, 40,4);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.drawString("Vysledek:", 0, 0, 4);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("PP:1:56,78", 0, 40,4);
  tft.setTextColor(TFT_WHITE);
  delay(WAIT);

  tft.fillScreen(TFT_BLACK);
  tft.drawString("posilam", 0, 0, 4);
  tft.drawString("na server", 0, 40,4);
  delay(WAIT);
}
