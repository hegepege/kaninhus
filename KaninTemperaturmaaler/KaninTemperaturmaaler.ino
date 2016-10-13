#define DHT_DEBUG
#include "DHT.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define USE_SERIAL Serial
#define DHTPIN D4     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE, 15);
int sample_delay = 2000;
float temp = 0;
float hum = 0;

//Network config
const char* ssid = "taletut";
const char* password = "xxxxxx";
const char* mqtt_server = "trygvis.io";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[10];

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 test!");
  randomSeed(analogRead(0));

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  dht.begin();
}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected to mqtt");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

   if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Wait a few seconds between measurements.
  delay(sample_delay);
     

  hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" *C ");
  Serial.print("\n");


  String temp_string =  String(temp, 2);
  temp_string.toCharArray(msg, 10); 
  Serial.println(msg);
  client.publish("vs11/kanin/temp", msg);

  
  String hum_string =  String(hum, 2);
  hum_string.toCharArray(msg, 10); 
  Serial.println(msg);
  client.publish("vs11/kanin/humidity", msg);

}
