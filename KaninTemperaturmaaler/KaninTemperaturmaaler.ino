#define DHT_DEBUG
#include "DHT.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor



#define USE_SERIAL Serial
#define DHTPIN D2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

ESP8266WiFiMulti WiFiMulti;
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE, 15);
int samples = 30;
int sample_delay = 1000;
int count = 0;
float temp = 0;
float hum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 test!");
  randomSeed(analogRead(0));

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiMulti.addAP("xxx", "xxx");

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(sample_delay);
     

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  hum += dht.readHumidity();
  // Read temperature as Celsius (the default)
  temp += dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  

  count +=1;
  Serial.print("Count: ");
  Serial.print(count);
  Serial.print("Humidity: ");
  Serial.print(hum/count);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temp/count);
  Serial.print(" *C ");
  Serial.print("\n");

  

  if(count == samples){
  String temp_reading = "humidity=";
  temp_reading += hum / samples;
  temp_reading += "\ntemp=";
  temp_reading += temp / samples;
  temp_reading += "\n";
  
  

   if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;
        String filename = "temp_reading_";
        filename += millis();
        filename += ".txt";
        
        http.begin("http://192.168.1.226:5005/webdav/"+filename); //HTTP
        http.setAuthorization("webdav", "xxx");
        Serial.print("[HTTP] PUT.."+filename+".\n");
      
        int httpCode = http.sendRequest("PUT", temp_reading);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
        } else {
            Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
    
    count = 0;
    hum = 0;
    temp = 0;
   
    
        }
}
