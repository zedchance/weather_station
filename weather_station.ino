/*
 * NODE MCU WEATHER STATION
 * Author   Zed Chance
 * 
 * PINS
 * OLED SCL       D2
 * OLED SDA       D1
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DEBUG

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Starting setup...");
#endif

  // OLED Setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.cp437(true); //font
  display_message("Setup..");
  delay(2000);
}

void loop()
{
  display_message("Ready.");
}

void display_message(String message)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}
