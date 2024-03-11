#include <WiFi.h> 
#include <Wire.h>

#include <Adafruit_Sensor.h> //Using Adafruit BMP Sensor Libraries
#include <Adafruit_BMP280.h> 
#include "ThingSpeak.h"

#define MOISTURE_SENSOR_PIN 33 // D33 pin on ESP32

char ssid[] = "Huloiarnata";   // your network SSID (name) 
char pass[] = "test1234";   // your network password
WiFiClient  client;

const float THRESHOLD = 50.0;

unsigned long myChannelNumber = 2462917;
const char * myWriteAPIKey = "PLDFKWER143MNEFY";

Adafruit_BMP280 bmp; // I2C

void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  if (!bmp.begin(0x76)) { // If different I2C address for the BMP280,change the address in the bmp.begin() function, commonly used address is 0x76, some modules might use the address 0x77.
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!")); //Checking for BMP Sensor
    while (1);
  }
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("[+] Attempting to connect to SSID: ");
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(200);     
    } 
    Serial.println("\n[+] Connected.");
  }

// Moisture Sensor code  
  int sensorValue = analogRead(MOISTURE_SENSOR_PIN);
  float moisture = map(sensorValue, 0, 4095, 0, 100); // map to percentage
  moisture = (moisture - 100) * -1;

  Serial.print("[*] Soil Moisture: ");
  Serial.print(moisture);
  Serial.println("%");

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeField(myChannelNumber, 1, moisture, myWriteAPIKey);
  if(x == 200){
    Serial.println("[+] Channel update successful.");
  }
  else{
    Serial.println("[*] Problem updating channel. HTTP error code " + String(x));
  }
// BMP280 Sensor Code
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  delay(2000); // Wait 2 seconds to update the channel again
}
