/*
    Programma: Test_oled_SSD1331_wemos_d1.ino
    
   This is a example sketch demonstrating the graphics
   capabilities of the SSD1331 library  for the 0.96"
   16-bit Color OLED with SSD1331 driver chip

    Ultima modifica il 10/5/2018

    Applicazione realizzata da Adriano Gandolfo
    Sito http://www.adrirobot.it
    Blog http://it.emcelettronica.com/author/adrirobot
    Pagina Facebook https://www.facebook.com/Adrirobot-318949048122955
    Istagram https://www.instagram.com/adrirobot/
    This example code is in the public domain.
*/
/*
// Definizione dei pin di collegamento
#define sclk 18
#define mosi 23
#define cs   17
#define rst  5
#define dc   16
*/
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

// Definizione dei colori
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_BMP280.h>

Adafruit_SSD1331 display = Adafruit_SSD1331(15, 2, 13, 14, 0);

Adafruit_BMP280 bmp; // I2C

const char* ssid = "Visitantes-GrupoEPM"; // Wifi SSID
const char* password = ""; // Wifi Password

//Your Domain name with URL path or IP address with path
String serverName = "https://gotempbmp280.herokuapp.com/sensor/156/temp/60";
String OldTemp = "";
String NewTemp = "";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 60 seconds (60000)
unsigned long timerDelay = 60000;

void setup(void) {
  display.begin();
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  Serial.begin(115200); 
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
   display.fillScreen(BLACK);
  display.setTextSize(2);
  display.setCursor(10, 0);
  display.setTextColor(WHITE);
  display.print("TEMP B4");
  display.setTextSize(1);
  display.drawFastHLine(0, 45, display.width() - 1, RED);
  display.setCursor(0, 52);
  display.setTextColor(GREEN);
  display.print("EDUARD MAZO");
  display.setTextSize(2);
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
    //Send an HTTP POST request every 10 minutes
    
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      NewTemp = bmp.readTemperature();
      String url = "http://gotempbmp280.herokuapp.com/sensor/1/temp/" + NewTemp;
      Serial.println(url);
      http.begin(client, url);      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
      display.setCursor(10, 16);
      display.setTextColor(BLACK);
      display.print(OldTemp);
      display.setCursor(10, 16);
      display.setTextColor(YELLOW);
      display.print(NewTemp);
      OldTemp = NewTemp;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

}
