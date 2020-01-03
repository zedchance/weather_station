/*
 * NODE MCU WEATHER STATION
 * Author   Zed Chance
 * 
 * Pin setup
 * DHT11          D0
 * OLED SDA       D1
 * OLED SCL       D2
 */

#include <SPI.h>                // OLED display stuff
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>             // DHT11 temp/humidity sensor

// OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT11
DHTesp dht;
//TaskHandle_t tempTaskHandle = NULL; // Task handle for the light value read task
int dhtPin = D0;                    // Pin number for DHT11 data pin

// Pins
#define temp_sensor D0

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // OLED Setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.cp437(true); //font
  display_message("OLED\nWEATHER\nSTATION..");
  delay(2000);

  // DHT11 Setup
  dht.setup(dhtPin, DHTesp::DHT11);
}

void loop()
{
  // Read DHT11
  TempAndHumidity read_dht = dht.getTempAndHumidity();
  String fahrenheit = String((read_dht.temperature * 1.8 + 32), 1) + "F";
  String humidity = String(read_dht.humidity, 0) + "%";
  display_message(fahrenheit + "  " + humidity);

  delay(2000);
}

void display_message(String message)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}
