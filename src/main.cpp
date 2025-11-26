#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <Wire.h> // for connecting I2C devices
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SGP30.h>

const char* ssid = "Robotics Institute of Kenya";
const char* password = "RObOT1C&#357";

// Your Flask server address (change this to match YOUR PC )
String serverURL = "http://192.168.1.56:5000/api/sgp30";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_SGP30 sgp30; // create SGP30 sensor object

void setup() {
  Serial.begin(115200); // start serial monitor
  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n WiFi connected");

  //initialize screen   
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); 
  sgp30.begin(); // initialize sgp30 sensor

  Serial.print("Found SGP30 serial #");
  Serial.print(sgp30.serialnumber[0], HEX);
  Serial.print(sgp30.serialnumber[1], HEX);
  Serial.println(sgp30.serialnumber[2], HEX);

  display.clearDisplay();
  display.display();
  delay(2000);

}

void loop() {
  //Read air quality values (eCO2 and TVOC)
  if (sgp30.IAQmeasure())
  {
    Serial.printf("CO2: %d TVOC: %d\n", sgp30.eCO2, sgp30.TVOC);
    // Serial.println(sgp30.eCO2);
    // Serial.println(sgp30.TVOC);
  }

  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);     
  display.printf("SGP30");
  display.setCursor(0, 20);     
  display.printf("CO2: %d \n", sgp30.eCO2); // same as: display.println("Temp: " + String(temp));
  display.setCursor(0, 40);     
  display.printf("TVOC: %d \n",sgp30.TVOC);
  display.display();

  //send data to database (mongodb)
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(serverURL);

    http.addHeader("Content-Type", "application/json");

    // JSON you send to Flask
    String jsonData = "{\"eCO2\":" + String(sgp30.eCO2) +
                      ",\"TVOC\":" + String(sgp30.TVOC)+"}";
 
    int httpResponseCode = http.POST(jsonData);

    Serial.print("Server Response: ");
    Serial.println(httpResponseCode);

    if(httpResponseCode > 0){
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.println("Error sending POST request");
    }

    http.end();
  }

  delay(3000); // Wait for 3 seconds before next reading

}

