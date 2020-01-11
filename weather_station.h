/*
 * Header file for weather_station.ino
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
NTPClient timeClient(ntpUDP); // 'pool.ntp.org' for time update
WiFiClient client;
const char* host = "192.168.50.84"; // raspberrypi
const uint16_t port = 5000;

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
