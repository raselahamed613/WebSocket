// #include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Inovace Engineering";
const char* password = "inovacetechIoT";
const char* mqtt_server = "103.115.255.11";  // Mosquitto broker
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const char* device_id = "d1234";

#define RELAY_PIN 2

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  // Copy payload to string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Payload: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.f_str());
    return;
  }

  const char* msg_device_id = doc["device_id"];
  const char* status = doc["status"];

  // Match device_id
  if (strcmp(msg_device_id, device_id) == 0) {
    if (strcmp(status, "ON") == 0) {
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Relay ON");
    } else if (strcmp(status, "OFF") == 0) {
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("Relay OFF");
    }
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

// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Connecting to MQTT...");
//     if (client.connect(device_id)) {
//       Serial.println("connected");

//       // Subscribe to topic
//       String topic = String("load/control/") + device_id;
//       client.subscribe(topic.c_str());
//       Serial.print("Subscribed to: ");
//       Serial.println(topic);
//     } else {
//       Serial.print("Failed. Retry in 5 seconds. State=");
//       Serial.println(client.state());
//       delay(5000);
//     }
//   }
// }

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}