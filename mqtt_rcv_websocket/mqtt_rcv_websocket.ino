// #include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Inovace Engineering";
const char* password = "inovacetechIoT";
const char* mqtt_server = "103.115.255.11";  // Mosquitto broker

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN 2

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) message += (char)payload[i];
  
  if (message == "ON") {
    digitalWrite(RELAY_PIN, HIGH);
  } else if (message == "OFF") {
    digitalWrite(RELAY_PIN, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      client.subscribe("load/control");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}
