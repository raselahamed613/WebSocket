// #include <ESP8266WiFi.h>
// #include <ESP8266WebServer.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "Inovace Engineering";
const char* password = "inovacetechIoT";

#define RELAY_PIN 2 // Use GPIO5 (D1)

// ESP8266WebServer server(80);
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

String webpage = R"rawlit(
<!DOCTYPE html>
<html>
<head>
  <title>Relay Control</title>
  <script>
    var socket = new WebSocket("ws://" + location.hostname + ":81/");
    function toggleRelay(state) {
      socket.send(state);
    }
  </script>
</head>
<body>
  <h1>Load Control</h1>
  <button onclick="toggleRelay('ON')">Turn ON</button>
  <button onclick="toggleRelay('OFF')">Turn OFF</button>
</body>
</html>
)rawlit";

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t len) {
  if (type == WStype_TEXT) {
    String msg = (char*)payload;
    Serial.println("WebSocket Message: " + msg);
    if (msg == "ON") {
      digitalWrite(RELAY_PIN, HIGH);
    } else if (msg == "OFF") {
      digitalWrite(RELAY_PIN, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // default off

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("Connected! IP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", webpage);
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}


