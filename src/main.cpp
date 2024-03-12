#include <Arduino.h>
#include <WiFi.h> 
#include <Wire.h>

#include <Adafruit_Sensor.h> //Using Adafruit BMP Sensor Libraries
#include <Adafruit_BMP280.h> 
#include "ThingSpeak.h"

#define MOISTURE_SENSOR_PIN 34 // A0 pin on ESP32

char ssid[] = "YOUR-SSID-HERE";   // your network SSID (name) 
char pass[] = "s0m3_l33t_p4ssw0rd";   // your network password
WiFiClient  client;

unsigned long myChannelNumber = 2465018;
const char * myWriteAPIKey = "API-KEY";

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

  // poll moisture data  
  int sensorValue = analogRead(MOISTURE_SENSOR_PIN);
  float moisture = (100 - ((sensorValue / 4095.00) * 100));   // soil moisture reading

  Serial.print("[*] Soil Moisture: ");
  Serial.print(moisture);
  Serial.println("%");

  ThingSpeak.setField(1, moisture);

  // poll sensor data
  float temperature = bmp.readTemperature();
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" *C");

  ThingSpeak.setField(2, temperature);

  // push data to thingspeak dashboard
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("[+] Channel update successful.");
    delay(500);
  }
  else{
    Serial.println("[*] Problem updating channel. HTTP error code " + String(x));
  }

  delay(2000); // Wait 2 seconds to update the channel again
}