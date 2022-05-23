
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


#include <BlynkSimpleEsp8266.h>

//wifi connect
const char* ssid = "Portable WIFI";
const char* password = "0714045300";

//node red server IP address
const char* mqtt_server = "192.168.1.23";

WiFiClient currentSensor;
PubSubClient client(currentSensor);

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

//connect esp to MQTT broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("currentSensor")) {
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

//current sensor connected to A0 pin
const int sensorIn = A0;

int mVperAmp = 185; 

double Voltage = 0;
double VRMS = 0;
double mAmpsRMS = 0;

BlynkTimer timer;

//function to read and write current values in json format
void readACS712() {
 Voltage = getVPP(); 
 mAmpsRMS = Voltage; //calculate power
 pinMode(sensorIn, INPUT);
 
 StaticJsonDocument<200> doc;
 doc["ID"]="ACS712";
 doc["Power"]=mAmpsRMS;
 char jsonBuffer[512];
 serializeJson(doc, jsonBuffer);
 Serial.println(jsonBuffer);
 client.publish("/acs712", jsonBuffer);
}

void setup(){ 
 Serial.begin(115200);
 timer.setInterval(1000, readACS712);
 setup_wifi();
 client.setServer(mqtt_server, 1883);
}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("currentSensor");
 Blynk.run();
 timer.run();
 }

//function to get current
float getVPP(){
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 100) { //sample for 1 Sec
       // see if you have a new maxValue
       readValue = analogRead(sensorIn);

       /*record the maximum sensor value*/  
       if (readValue > maxValue) { 
           maxValue = readValue;
       }
       /*record the maximum sensor value*/
       if (readValue < minValue) {
           minValue = readValue;
       }
   }
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }
