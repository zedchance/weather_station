/*
 * NODE MCU WEATHER STATION
 * Author   Zed Chance
 *
 * Shows the indoor temp and humidity, as well as the outdoor temp/humidity and
 * conditions. The Node communicates with a Rasbperry Pi, which does the pulling
 * and serving of the data.
 *
 * Pin setup
 * DHT11          D0
 * SDA            D1
 * SCL            D2
 */

#include "weather_station.h"

// Modes
#define DEBUG       // ON/OFF debug statements
#define CYCLE       // Cycles thru all stats
// #define DUAL_TEMP   // Shows indoor and outdoor temp

int total_stats;
unsigned long last_millis = 0;
String otemp;
String ohumidity;
String oconditions;
String bot_status_str;

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("Starting setup...");
#endif

    otemp = "N/A";
    ohumidity = "N/A";
    oconditions = "N/A";

    // OLED Setup
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.cp437(true); //font
    display.dim(true);
    display_message("OLED\nWEATHER\nSTATION");
    delay(2000);

    // Wifi and time setup
    display_message("Connecting\nto wifi.");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay ( 500 );
    }
    timeClient.begin();
    timeClient.setTimeOffset(-28800);
    display_message("Connected.");
    delay(500);
    display.clearDisplay();

    // DHT11 Setup
    dht.setup(dhtPin, DHTesp::DHT11);

    // BMP180 Setup
    // Wire.begin();
    // if (!bmp180.begin())
    // {
    //     Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    //     while (1);
    // }
    // bmp180.resetToDefaults();
    // bmp180.setSamplingMode(BMP180MI::MODE_UHR);

    // Pull Darksky weather and bot status
    total_stats = 2;
    pull_weather(&total_stats);
    pull_bot_status(&total_stats);
}

void loop()
{
    display.clearDisplay();

    // Cycle mode
#ifdef CYCLE
    int pause = 3000;
    for (int i = 0; i < total_stats; i++)
    {
        update_time();
        print_stats(i);
        progress_bar(i, total_stats);
        display.clearDisplay();
    }
#endif

    // Shows indoor/outdoor temp constantly
#ifdef DUAL_TEMP
    update_time();
    both_temps();
    display.display();
    delay(5000);
    display.clearDisplay();
#endif

    // Pull time every 30 seconds
    unsigned long time_wait_duration = 30000;
    if ((unsigned long)(millis() - last_millis >= time_wait_duration))
    {
        timeClient.update();
    }

    // Check if its time to pull
    unsigned long wait_duration = 300000; // 5 min
    if ((unsigned long)(millis() - last_millis >= wait_duration))
    {
        Serial.println("Repull.");
        total_stats = 2;
        pull_weather(&total_stats);
        pull_bot_status(&total_stats);
        last_millis = millis();
    }
}

void update_time()
{
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
        case 5:
            bot_status();
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

void progress_bar(int index, int total)
{
    // Ticks
    int chunk = (SCREEN_WIDTH - 1) / total;
    for (int i = 0; i < SCREEN_WIDTH - 1; i += chunk)
    {
        display.drawPixel(i, 62, SSD1306_WHITE);
    }

    // Progress bar
    for (int i = 0; i < index * chunk; i++)
    {
        display.drawPixel(i, 63, SSD1306_WHITE);
    }
    for (int i = index * chunk; i < (index + 1) * chunk; i++)
    {
        display.drawPixel(i, 63, SSD1306_WHITE);
        display.display();
        delay(250);
    }
}

void indoor_temp()
{
    TempAndHumidity read_dht = dht.getTempAndHumidity();
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("Inside temp");
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
    display.print("Inside humidity");
    display.setCursor(0, 19);
    String humidity = String(read_dht.humidity, 0) + "%";
    display.setTextSize(4);
    display.println(humidity);
}

void outdoor_temp()
{
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("Outside temp");
    display.setCursor(0, 19);
    display.setTextSize(4);
    display.println(otemp);
}

void outdoor_humidity()
{
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("Outside humidity");
    display.setCursor(0, 19);
    display.setTextSize(4);
    display.println(ohumidity);
}

void outdoor_conditions()
{
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("Outside conditions");
    display.setCursor(0, 19);
    display.setTextSize(2);
    display.println(oconditions);
}

void both_temps()
{
    TempAndHumidity read_dht = dht.getTempAndHumidity();
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print(oconditions);
    display.setCursor(0, 19);
    String fahrenheit = String((read_dht.temperature * 1.8 + 32), 1) + "F";
    display.setTextSize(2);
    display.println("I: " + fahrenheit);
    display.println("O: " + otemp);
}

void bot_status()
{
    display.setTextSize(1);
    display.setCursor(0, 52);
    display.print("!b status");
    display.setCursor(0, 19);
    display.setTextSize(2);
    display.println(bot_status_str);
}

void pull_weather(int *s)
{
    // Connect client to host and check for failure
    Serial.println("Pulling weather");
    display_message("Pulling\nweather");
    if (!client.connect(host, port))
    {
        Serial.println("Pull failed");
        display_message("Pull\nfailed");
        otemp = "N/A";
        ohumidity = "N/A";
        oconditions = "N/A";
        delay(5000);
        return;
    }

    // Make GET request
    if (client.connected())
    {
        // Make request for forecast for Roseville, CA
        client.println("C\n");
    }

    // Wait 5s for response
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

    // Read and parse response
    int c = 0;
    String in = "";
    while (client.available())      // Read in response char by char
    {
        char ch = static_cast<char>(client.read());
        if (ch == ' ') in += '\n';  // If there is a space, then add a newline
        else if (ch == '\n')        // If its a newline, then store it away
        {
            switch (c)
            {
            case 0:                 // First part is temp
                otemp = in;
                break;
            case 1:                 // Second part is humidity
                ohumidity = in;
                break;
            case 2:                 // Finally conditions
                oconditions = in;
                break;
            }
            c++;                    // Move to next stat
            in = "";                // Reset input string
        }
        else in += ch;              // Build up input string
        Serial.print(ch);
    }
    *s += 3;                        // Add 3 stats to total
    display_message("Pulled.");
    delay(500);

    // Close connection
    client.stop();
    Serial.println("Connection closed.");
}

void pull_bot_status(int *s)
{
    Serial.println("Pulling bot status");
    display_message("Pulling\nbot status");
    if (!client.connect(host, port))
    {
        Serial.println("Pull failed");
        display_message("Pull\nfailed");
        bot_status_str = "N/A";
        delay(5000);
        return;
    }

    if (client.connected())
    {
        // Make request for bot status
        client.println("BBOT\n");
    }

    // Wait 5s for response
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

    // Read and parse response
    String in = "";
    while (client.available())      // Read in response char by char
    {
        char ch = static_cast<char>(client.read());
        if (ch == '\n')             // If its a newline, then store it away
        {
            bot_status_str = in;
        }
        else in += ch;              // Otherwise build up input string
        Serial.print(ch);
    }
    *s += 1;                        // Add 1 stat to total
    display_message("Pulled.");
    delay(500);

    // Close connection
    client.stop();
    Serial.println("Connection closed.");
}
