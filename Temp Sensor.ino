#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 5     
#define DHTTYPE DHT11

//DHT pit type define
DHT dht(DHTPIN, DHTTYPE);

//wifi connect
const char* ssid = "Portable WIFI";
const char* password = "0714045300";
const char* mqtt_server = "192.168.1.23";

WiFiClient espClient;
PubSubClient client(espClient);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

//connect esp to wifi
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
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

//connecting to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 test!"));
  setup_wifi();
  dht.begin();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();

  //send data with 30 seconds delay
  if (now - lastMeasure > 35000) {
    
    lastMeasure = now;
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
      }
      
    //display data in json format
    StaticJsonDocument<200> doc;
    doc["ID"]="DHT-11";
    doc["Temp"]=t;
    doc["Humid"]=h;
    char jsonBuffer[512];
    serializeJson(doc, jsonBuffer);
  
    Serial.println(jsonBuffer);
    client.publish("/dht11/1", jsonBuffer);   
    }
}
