/*
 * NODE MCU WEATHER STATION
 * Author   Zed Chance
 * 
 * Pin setup
 * DHT11          D0
 * SDA            D1
 * SCL            D2
 */

#include <string.h>
#include <SPI.h>
#include <Wire.h>

// Wifi and time setup
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "wifi_auth.h"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); // 'pool.ntp.org' is used with 60 seconds update interval

// OLED
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT11
#include <DHTesp.h>
DHTesp dht;
int dhtPin = D0; // Pin number for DHT11 data pin

// BMP180
#include <BMP180I2C.h>
#define BMP_ADDRESS 0x77
BMP180I2C bmp180(BMP_ADDRESS); //create an BMP180 object using the I2C interface

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // OLED Setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.cp437(true); //font
  display_message("OLED\nWEATHER\nSTATION");
  delay(2000);

  // Wifi and time setup
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Connecting");
  display.display();
  display.startscrollleft(0x00, 0xFF);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay ( 500 );
  }
  timeClient.begin();
  timeClient.setTimeOffset(-28800);
  display.stopscroll();
  display.clearDisplay();
  display.setCursor(0, 0);
  Serial.println("Time synced using SSID: " + String(ssid));
  delay(1000);
  display.clearDisplay();

  // DHT11 Setup
  dht.setup(dhtPin, DHTesp::DHT11);

  // BMP180 Setup
//  Wire.begin();
//  if (!bmp180.begin())
//  {
//    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
//    while (1);
//  }
//  bmp180.resetToDefaults();
//  bmp180.setSamplingMode(BMP180MI::MODE_UHR);
}

void loop()
{
  // Time
  timeClient.update();
  display.clearDisplay();
  display.setCursor(0, 0);
  String hours = String(timeClient.getHours());
  String mins = String(timeClient.getMinutes());
  display.println(hours + ":" + mins);

  // Read DHT11
  TempAndHumidity read_dht = dht.getTempAndHumidity();

  // Print weather stats
  print_stats(&read_dht);

  // Read BMP180
//  if (!bmp180.measureTemperature()) Serial.println("No temp on BMP180");
//  while (!bmp180.hasValue()) delay(100);
//  if (!bmp180.measurePressure()) Serial.println("No pressure on BMP180");
//  while (!bmp180.hasValue()) delay(100);
//  disp += String(bmp180.getPressure()) + " Pa";

  // Wait
  delay(2000);
}

void print_stats(TempAndHumidity *t)
{
  display.setCursor(0, 18);

  String fahrenheit = String((t->temperature * 1.8 + 32), 1) + "F";
  String humidity = String(t->humidity, 0) + "%";
  display.setTextSize(1);
  display.println("Temperature");
  display.setTextSize(2);
  display.println(fahrenheit);
  display.setTextSize(1);
  display.println("Humidity");
  display.setTextSize(2);
  display.println(humidity);

  display.display();
}

void display_message(String message)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}
