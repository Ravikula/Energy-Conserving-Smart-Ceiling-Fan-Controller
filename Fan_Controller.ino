#include <ESP8266WiFi.h>
#include <PubSubClient.h>

String opMode = "auto";

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "Portable WIFI";
const char* password = "0714045300";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.1.23";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient fanController;
PubSubClient client(fanController);

// Lamp - LED - GPIO 4 = D2 on ESP-12E NodeMCU board
#define relay1  16
#define relay2  5
#define relay3  4
#define relay4  0

// Don't change the function below. This functions connects your ESP8266 to your router
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

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String controlMsg;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    controlMsg += (char)message[i];
  }
  Serial.println();

//checking the operating mode of the controller
  if(topic=="controller/mode"){
      Serial.print("changing mode to ");
      if(controlMsg == "auto"){
        opMode="auto";
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);      
      }
      else if(controlMsg == "manual"){
        opMode="manual";
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
  }

  // logic to activate relays in auto mode
  if(topic=="fan/controller/auto" && opMode=="auto"){
      Serial.print("Automatic mode-Changing speed to ");
      if(controlMsg == "0"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "1"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
        delay(500);
        digitalWrite(relay4, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "2"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay4, HIGH);
        delay(500);
        digitalWrite(relay3, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "3"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        delay(500);
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "4"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay4, HIGH);
        delay(500);
        digitalWrite(relay3, LOW);
        digitalWrite(relay2, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "5"){
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        delay(500);
        digitalWrite(relay1, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
  }

//logic to activate relays in manual mode
  else if(topic=="fan/controller/manual" && opMode=="manual"){
    Serial.print("Manual mode-Changing speed to ");
      if(controlMsg == "0"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "1"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
        delay(500);
        digitalWrite(relay4, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "2"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        digitalWrite(relay4, HIGH);
        delay(500);
        digitalWrite(relay3, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "3"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);
        delay(500);
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "4"){
        digitalWrite(relay1, HIGH);
        digitalWrite(relay4, HIGH);
        delay(500);
        digitalWrite(relay3, LOW);
        digitalWrite(relay2, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
      if(controlMsg == "5"){
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
        digitalWrite(relay4, HIGH);
        delay(500);
        digitalWrite(relay1, LOW);
        Serial.println(controlMsg);
        Serial.print("Operating Mode: ");
        Serial.println(opMode);
      }
  }
  Serial.println();
}

// This functions reconnects ESP8266 to your MQTT broker 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("fanController")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("fan/controller/auto");
      client.subscribe("fan/controller/manual");
      client.subscribe("controller/mode");
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
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()){
    client.connect("fanController");
  } 
}
