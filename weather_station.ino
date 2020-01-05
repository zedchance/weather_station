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

#define DEBUG

// Wifi and time setup
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "wifi_auth.h"
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); // 'pool.ntp.org' is used with 60 seconds update interval
WiFiClient client;
const char* host = "raspberrypi";
const uint16_t port = 1234;

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
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("Starting setup...");
#endif

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

  // Pull Darksky weather
  pull_weather();
}

void loop()
{
  display.clearDisplay();
  
  // Cycle stats
  int total_stats = 5;
  int pause = 3000;
  for (int i = 0; i < total_stats; i++)
  {
    update_time();
    print_stats(i);
    delay(pause);
    display.clearDisplay();
  }
}

void update_time()
{
  timeClient.update();
  display.setCursor(0, 0);
  display.setTextSize(2);
  int h = timeClient.getHours();
  if (h > 12) h -= 12;
  String hours = String(h);
  int m = timeClient.getMinutes();
  String mins = String(m);
  if (m < 10) mins = "0" + mins;
  display.print(hours + ":" + mins);
}

void print_stats(int stat)
{
  switch(stat)
  {
    case 0:
      indoor_temp();
      break;
    case 1:
      indoor_humidity();
      break;
    case 2:
      outdoor_temp();
      break;
    case 3:
      outdoor_humidity();
      break;
    case 4:
      outdoor_conditions();
      break;
    default:
      break;
  }
  display.display();
}

void display_message(String message)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

void indoor_temp()
{
  TempAndHumidity read_dht = dht.getTempAndHumidity();
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Indoors temp");
  display.setCursor(0, 19);
  String fahrenheit = String((read_dht.temperature * 1.8 + 32), 1) + "F";
  display.setTextSize(4);
  display.println(fahrenheit);
}

void indoor_humidity()
{
  TempAndHumidity read_dht = dht.getTempAndHumidity();
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Indoors humidity");
  display.setCursor(0, 19);
  String humidity = String(read_dht.humidity, 0) + "%";
  display.setTextSize(4);
  display.println(humidity);
}

void outdoor_temp()
{
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Outdoors temp");
  display.setCursor(0, 19);
  // get temp
  display.setTextSize(4);
  display.println("");
}

void outdoor_humidity()
{
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Outdoors humidity");
  display.setCursor(0, 19);
  // get humidity
  display.setTextSize(4);
  display.println("");
}

void outdoor_conditions()
{
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print("Outdoors conditions");
  display.setCursor(0, 19);
  // get conditions
  display.setTextSize(4);
  display.println("");
}

void pull_weather()
{
  // Connect client to host and check for failure
  if (!client.connect(host, port))
  {
    Serial.println("Connection failed");
    display_message("Connection\nfailed");
    delay(5000);
    return;
  }

  // Make GET request
  if (client.connected())
  {
    // Make request for forecast for Roseville, CA
    client.println("G");
  }

  // Wait for response
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println("CLIENT TIMEOUT!");
      display_message("Client\ntimeout");
      client.stop();
      delay(10000);
      return;
    }
  }

  // Read response
  while (client.available())
  {
    char ch = static_cast<char>(client.read());
    Serial.print(ch);
  }

  // Close connection
  client.stop();
  Serial.println("Connection closed.");
}
